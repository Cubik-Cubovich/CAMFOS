.section .text

.macro isr_no_errno num
.global isr\num
isr\num:
    pushq $0
    pushq $\num
    jmp isr_common_stub
.endm

.macro isr_errno num
.global isr\num
isr\num:
    pushq $\num
    jmp isr_common_stub
.endm

isr_common_stub:
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %rbx
    pushq %rbp
    pushq %rsi
    pushq %rdi
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    pushq %rax
    mov $'A', %al
    out %al, $0xE9
    popq %rax

    movq %rsp, %rdi
    call isr_handler_c

    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdi
    popq %rsi
    popq %rbp
    popq %rbx
    popq %rdx
    popq %rcx
    popq %rax

    addq $16, %rsp
    iretq

# Исключения 0–31
isr_no_errno 0
isr_no_errno 1
isr_no_errno 2
isr_no_errno 3
isr_no_errno 4
isr_no_errno 5
isr_no_errno 6
isr_no_errno 7
isr_errno    8
isr_no_errno 9
isr_errno    10
isr_errno    11
isr_errno    12
isr_errno    13
isr_errno    14
isr_no_errno 15
isr_no_errno 16
isr_errno    17
isr_no_errno 18
isr_no_errno 19
isr_no_errno 20
isr_no_errno 21
isr_no_errno 22
isr_no_errno 23
isr_no_errno 24
isr_no_errno 25
isr_no_errno 26
isr_no_errno 27
isr_no_errno 28
isr_no_errno 29
isr_errno    30
isr_no_errno 31

# IRQ (векторы 32–47)
isr_no_errno 32
isr_no_errno 33
isr_no_errno 34
isr_no_errno 35
isr_no_errno 36
isr_no_errno 37
isr_no_errno 38
isr_no_errno 39
isr_no_errno 40
isr_no_errno 41
isr_no_errno 42
isr_no_errno 43
isr_no_errno 44
isr_no_errno 45
isr_no_errno 46
isr_no_errno 47

.global isr_default_stub
isr_default_stub:
    pushq $0
    pushq $0xFF
    jmp isr_common_stub

.global load_idt
load_idt:
    lidt (%rdi)
    ret