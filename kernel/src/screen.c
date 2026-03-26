#include "screen.h"
#include "font.h"
#include <stddef.h>

static uint32_t *framebuffer;
static uint32_t pitch;
static uint32_t width;
static uint32_t height;

void screen_init(uint32_t *fb, uint32_t p, uint32_t w, uint32_t h) {
    framebuffer = fb;
    pitch = p;
    width = w;
    height = h;
}

void draw_char(int x, int y, char c, uint32_t color) {
    if (c < '0' || c > '9') return;
    int idx = c - '0';
    const uint8_t *glyph = font_digits[idx];
    for (int row = 0; row < 8; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < 8; col++) {
            if (bits & (1 << (7 - col))) {
                int px = x + col;
                int py = y + row;
                if (px >= 0 && px < (int)width && py >= 0 && py < (int)height) {
                    framebuffer[py * (pitch / 4) + px] = color;
                }
            }
        }
    }
}

void draw_string(int x, int y, const char *str, uint32_t color) {
    int cur_x = x;
    for (size_t i = 0; str[i] != '\0'; i++) {
        draw_char(cur_x, y, str[i], color);
        cur_x += 8; // ширина символа
    }
}

void draw_number(int x, int y, uint64_t num, uint32_t color) {
    char buffer[21];
    int i = 20;
    buffer[i] = '\0';
    if (num == 0) {
        buffer[--i] = '0';
    } else {
        while (num > 0) {
            buffer[--i] = '0' + (num % 10);
            num /= 10;
        }
    }
    draw_string(x, y, &buffer[i], color);
}