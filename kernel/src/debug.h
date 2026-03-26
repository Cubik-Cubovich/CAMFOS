#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

static inline void debug_char(char c) {
    asm volatile("outb %0, $0xE9" : : "a"(c));
}

static inline void debug_str(const char *str) {
    while (*str) debug_char(*str++);
}

static inline void debug_num(uint64_t num) {
    char buf[21];
    int i = 20;
    buf[i] = '\0';
    if (num == 0) {
        buf[--i] = '0';
    } else {
        while (num > 0) {
            buf[--i] = '0' + (num % 10);
            num /= 10;
        }
    }
    debug_str(&buf[i]);
}

#endif