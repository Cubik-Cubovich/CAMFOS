#include "idt.h"
#include "debug.h"

static struct idt_entry idt[256];
static struct idt_ptr idt_ptr;
static uint16_t current_cs = 0;

extern void isr_default_stub(void);

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
    debug_char('\n');
}