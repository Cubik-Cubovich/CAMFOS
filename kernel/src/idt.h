#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attr;
    uint16_t base_mid;
    uint32_t base_high;
    uint32_t reserved;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

extern void load_idt(struct idt_ptr *ptr);
void idt_init(void);
void idt_set_gate(uint8_t vector, void *handler, uint8_t type_attr);
uint64_t idt_get_gate_address(uint8_t vector);

#endif