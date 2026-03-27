#include "interrupts.h"
#include "idt.h"
#include "pic.h"
#include "io.h"
#include "debug.h"

static void (*interrupt_handlers[256])(interrupt_frame_t *frame) = {0};

static void default_handler(interrupt_frame_t *frame) {
    debug_str("Unhandled int: ");
    debug_num(frame->int_no);
    debug_char('\n');
    for (;;) asm volatile("hlt");
}

void register_interrupt_handler(uint8_t vector, void (*handler)(interrupt_frame_t *frame)) {
    interrupt_handlers[vector] = handler;
}

void isr_handler_c(interrupt_frame_t *frame) {
    debug_str("frame addr: ");
    debug_num((uint64_t)frame);
    debug_char('\n');
    debug_str("int_no at offset: ");
    debug_num((uint64_t)&frame->int_no);
    debug_char('\n');
    debug_str("int_no value: ");
    debug_num(frame->int_no);
    debug_char('\n');
    debug_char('C');
    void (*handler)(interrupt_frame_t *) = interrupt_handlers[frame->int_no];
    if (handler)
        handler(frame);
    else
        default_handler(frame);
}

void enable_interrupts(void) {
    asm volatile("sti");
}

void disable_interrupts(void) {
    asm volatile("cli");
}

void init_interrupts(void) {
    idt_init();
    debug_str("IDT loaded\n");

    pic_init();
    debug_str("PIC initialized\n");

    // Разрешаем IRQ0 (таймер) и IRQ1 (клавиатура) – для примера
    pic_mask_irq(0, false);
    pic_mask_irq(1, false);
    debug_str("IRQ0, IRQ1 unmasked\n");

    // Выводим маску master PIC
    uint8_t master_mask = inb(PIC_MASTER_DATA);
    debug_str("PIC master mask = ");
    debug_num(master_mask);
    debug_char('\n');
    if ((master_mask & 1) == 0) {
        debug_str("IRQ0 is ENABLED (good)\n");
    } else {
        debug_str("IRQ0 is DISABLED (bad)\n");
    }
}