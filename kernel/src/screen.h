#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

void screen_init(uint32_t *fb, uint32_t pitch, uint32_t width, uint32_t height);
void draw_char(int x, int y, char c, uint32_t color);
void draw_string(int x, int y, const char *str, uint32_t color);
void draw_number(int x, int y, uint64_t num, uint32_t color);

#endif