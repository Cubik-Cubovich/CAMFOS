#include "idt.h"
#include "debug.h"

static struct idt_entry idt[256];
static struct idt_ptr idt_ptr;
static uint16_t current_cs = 0;

extern void isr_default_stub(void);

uint64_t idt_get_gate_address(uint8_t vector) {
    uint64_t low = idt[vector].base_low;
    uint64_t mid = idt[vector].base_mid;
    uint64_t high = idt[vector].base_high;
    return low | (mid << 16) | (high << 32);
}

void idt_init(void) {
    // текущий CS
    asm volatile("mov %%cs, %0" : "=r"(current_cs));
    debug_str("Current CS: ");
    debug_num(current_cs);
    debug_char('\n');
    
    // заполнение IDT заглушками
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, isr_default_stub, 0x8E);
    }

    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint64_t)&idt[0];
    
    debug_str("IDT base: ");
    debug_num(idt_ptr.base);
    debug_char('\n');
    debug_str("IDT limit: ");
    debug_num(idt_ptr.limit);
    debug_char('\n');

    // Загрузка IDT
    load_idt(&idt_ptr);
    debug_str("IDT loaded\n");
    
    // Проверка: читаем IDT обратно из процессора
    struct idt_ptr check;
    asm volatile("sidt %0" : "=m"(check));
    debug_str("SIDT base: ");
    debug_num(check.base);
    debug_char('\n');
    debug_str("SIDT limit: ");
    debug_num(check.limit);
    debug_char('\n');
    
    if (check.base != (uint64_t)&idt) {
        debug_str("WARNING: Loaded IDT base != expected!\n");
    }
    struct {
        uint16_t limit;
        uint64_t base;
    } __attribute__((packed)) gdtr;
    asm volatile("sgdt %0" : "=m"(gdtr));
    debug_str("GDT base: ");
    debug_num(gdtr.base);
    debug_char('\n');
    debug_str("GDT limit: ");
    debug_num(gdtr.limit);
    debug_char('\n');
    
    // Выводим информацию о селекторе 40 (0x28)
    // Селектор 40 = индекс 8 (40/8 = 5, потому что 8 байт на дескриптор)
    uint64_t *gdt = (uint64_t*)gdtr.base;
    uint64_t desc = gdt[5];  // индекс 5, так как 40/8 = 5
    debug_str("GDT entry for selector 40: ");
    debug_num(desc);
    debug_char('\n');
    
    // Проверяем биты
    debug_str("  Type: ");
    debug_num((desc >> 40) & 0xF);
    debug_char('\n');
    debug_str("  Present: ");
    debug_num((desc >> 47) & 1);
    debug_char('\n');
    debug_str("  Long mode: ");
    debug_num((desc >> 53) & 1);
    debug_char('\n');
    debug_str("  D/B: ");
    debug_num((desc >> 54) & 1);
    debug_char('\n');
}

void idt_set_gate(uint8_t vector, void *handler, uint8_t type_attr) {
    uint64_t addr = (uint64_t)handler;
    idt[vector].base_low  = addr & 0xFFFF;
    idt[vector].selector  = current_cs;
    idt[vector].ist       = 0;
    idt[vector].type_attr = type_attr;
    idt[vector].base_mid  = (addr >> 16) & 0xFFFF;
    idt[vector].base_high = (addr >> 32) & 0xFFFFFFFF;
    idt[vector].reserved  = 0;
    
    debug_str("IDT gate set: vector ");
    debug_num(vector);
    debug_str(" selector ");
    debug_num(current_cs);
    debug_str(" type_attr ");
    debug_num(type_attr);
    debug_char('\n');
}