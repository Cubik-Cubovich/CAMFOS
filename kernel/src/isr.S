.section .text

.macro isr_no_errno num
.global isr\num
isr\num:
    # Отладка: вывести номер вектора (для 32 будет '2', для 33 '3' и т.д.)
    pushq %rax
    mov $\num, %al
    cmp $10, %al
    jl 1f
    # Для чисел >=10 выводим две цифры (упрощённо)
    pushq %rbx
    mov $10, %bl
    div %bl
    add $'0', %al
    out %al, $0xE9
    mov %ah, %al
    add $'0', %al
    out %al, $0xE9
    popq %rbx
    jmp 2f
1:
    add $'0', %al
    out %al, $0xE9
2:
    popq %rax
    
    pushq $0
    pushq $\num
    jmp isr_common_stub
.endm

.macro isr_errno num
.global isr\num
isr\num:
    # Отладка: вывести номер вектора
    pushq %rax
    mov $\num, %al
    cmp $10, %al
    jl 3f
    pushq %rbx
    mov $10, %bl
    div %bl
    add $'0', %al
    out %al, $0xE9
    mov %ah, %al
    add $'0', %al
    out %al, $0xE9
    popq %rbx
    jmp 4f
3:
    add $'0', %al
    out %al, $0xE9
4:
    popq %rax
    
    pushq $\num
    jmp isr_common_stub
.endm

isr_common_stub:
    # Сохраняем регистры
    pushq %r15
    pushq %r14
    pushq %r13
    pushq %r12
    pushq %r11
    pushq %r10
    pushq %r9
    pushq %r8
    pushq %rdi
    pushq %rsi
    pushq %rbp
    pushq %rbx
    pushq %rdx
    pushq %rcx
    pushq %rax

    # Вызываем C-обработчик
    movq %rsp, %rdi
    call isr_handler_c

    # Восстанавливаем регистры
    popq %rax
    popq %rcx
    popq %rdx
    popq %rbx
    popq %rbp
    popq %rsi
    popq %rdi
    popq %r8
    popq %r9
    popq %r10
    popq %r11
    popq %r12
    popq %r13
    popq %r14
    popq %r15

    addq $16, %rsp   # убираем int_no и err_code
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
    # Отладка: вывести 'D' для default
    pushq %rax
    mov $'D', %al
    out %al, $0xE9
    popq %rax
    
    pushq $0
    pushq $0xFF
    jmp isr_common_stub

.global load_idt
load_idt:
    lidt (%rdi)
    ret