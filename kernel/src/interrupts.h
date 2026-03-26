#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

struct interrupt_frame {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, rflags, rsp, ss;
};

typedef struct interrupt_frame interrupt_frame_t;

void enable_interrupts(void);
void disable_interrupts(void);
void register_interrupt_handler(uint8_t vector, void (*handler)(interrupt_frame_t *frame));
void init_interrupts(void);

#endif