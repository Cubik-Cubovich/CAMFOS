#include "timer.h"
#include "interrupts.h"
#include "pic.h"
#include "io.h"
#include "idt.h"
#include "debug.h"

extern void isr32(void);

volatile uint64_t ticks = 0;
static uint32_t timer_divisor = 0;

void timer_handler(interrupt_frame_t *frame) {
    (void)frame;
    ticks++;
    debug_char('T');  // если увидишь T, значит прерывания приходят
    pic_send_eoi(0);
}

void init_timer(uint32_t frequency) {
    timer_divisor = 1193182 / frequency;
    
    outb(0x43, 0x34);
    outb(0x40, timer_divisor & 0xFF);
    outb(0x40, (timer_divisor >> 8) & 0xFF);
    
    debug_str("PIT divisor: ");
    debug_num(timer_divisor);
    debug_char('\n');
    
    idt_set_gate(32, isr32, 0x8E);
    register_interrupt_handler(32, timer_handler);

    outb(0x43, 0x00);  // latch counter
    uint16_t low = inb(0x40);
    uint16_t high = inb(0x40);
    uint16_t count = low | (high << 8);
    debug_str("PIT count: ");
    debug_num(count);
    debug_char('\n');

    debug_str("Calling isr32 directly...\n");
    isr32();
    debug_str("Back from isr32\n");

    // Читаем IDT через SIDT
    struct idt_ptr current_idt;
    asm volatile("sidt %0" : "=m"(current_idt));
    struct idt_entry *idt_entries = (struct idt_entry*)current_idt.base;
    
    // Выводим сырые байты IDT[32]
    uint8_t *raw = (uint8_t*)&idt_entries[32];
    debug_str("IDT[32] raw bytes: ");
    for (int i = 0; i < 16; i++) {
        debug_num(raw[i]);
        debug_char(' ');
    }
    debug_char('\n');
    
    // Вычисляем адрес из IDT
    uint64_t addr_from_idt = idt_entries[32].base_low | 
                              ((uint64_t)idt_entries[32].base_mid << 16) | 
                              ((uint64_t)idt_entries[32].base_high << 32);
    
    debug_str("IDT[32] from CPU: ");
    debug_num(addr_from_idt);
    debug_char('\n');
    debug_str("isr32 addr: ");
    debug_num((uint64_t)isr32);
    debug_char('\n');
    
    if (addr_from_idt != (uint64_t)isr32) {
        debug_str("ERROR: CPU sees different address!\n");
    }
    
    // Проверка через функцию idt_get_gate_address
    uint64_t idt_addr = idt_get_gate_address(32);
    debug_str("IDT[32] points to: ");
    debug_num(idt_addr);
    debug_char('\n');
    debug_str("isr32 is at: ");
    debug_num((uint64_t)isr32);
    debug_char('\n');
    
    if (idt_addr == (uint64_t)isr32) {
        debug_str("OK: IDT points to isr32\n");
    } else {
        debug_str("ERROR: IDT points elsewhere!\n");
        debug_str("Diff: ");
        debug_num((uint64_t)isr32 - idt_addr);
        debug_char('\n');
    }
    
    debug_str("Timer initialized at ");
    debug_num(frequency);
    debug_str(" Hz\n");
    
    // Программное прерывание для теста
    asm volatile("int $0x32");
}