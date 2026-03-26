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
    debug_char('T');
    
    // Перезагружаем PIT
    outb(0x40, timer_divisor & 0xFF);
    outb(0x40, (timer_divisor >> 8) & 0xFF);
    
    pic_send_eoi(0);
    debug_char('E');
}

void init_timer(uint32_t frequency) {
    timer_divisor = 1193182 / frequency;
    
    outb(0x43, 0x34);   // Режим 2 (rate generator)
    outb(0x40, timer_divisor & 0xFF);
    outb(0x40, (timer_divisor >> 8) & 0xFF);
    
    debug_str("PIT divisor: ");
    debug_num(timer_divisor);
    debug_char('\n');
    
    idt_set_gate(32, isr32, 0x8E);
    register_interrupt_handler(32, timer_handler);
    
    debug_str("Timer initialized at ");
    debug_num(frequency);
    debug_str(" Hz\n");
}