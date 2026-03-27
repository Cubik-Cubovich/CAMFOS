#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include "interrupts.h"
#include "timer.h"  
#include "debug.h"
#include "io.h"  

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

static void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}

void kmain(void) {
    uint16_t cs;
    asm volatile("mov %%cs, %0" : "=r"(cs));
    debug_str("CS in kmain: ");
    debug_num(cs);
    debug_char('\n');
    // CPL = cs & 3
    debug_str("CPL: ");
    debug_num(cs & 3);
    debug_char('\n');

    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) hcf();
    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) hcf();

    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    // Инициализация прерываний
    init_interrupts();
    
    // Настройка таймера
    init_timer(1000);
    
    // Включаем прерывания
    enable_interrupts();
    asm volatile("int $0x32");
    uint64_t flags;
    asm volatile("pushf; pop %0" : "=r"(flags));
    debug_str("RFLAGS: ");
    debug_num(flags);
    debug_char('\n');

    uint64_t rflags;
    asm volatile("pushf; pop %0" : "=r"(rflags));
    debug_str("RFLAGS: ");
    debug_num(rflags);
    debug_char('\n');
    
    // Рисуем градиент
    volatile uint32_t *fb_ptr = framebuffer->address;
    for (size_t y = 0; y < framebuffer->height; y++) {
        for (size_t x = 0; x < framebuffer->width; x++) {
            uint32_t nX = x * 255 / framebuffer->width;
            uint32_t nY = y * 255 / framebuffer->height;
            fb_ptr[y * (framebuffer->pitch / 4) + x] = (nY << 8) | nX;
        }
    }
    
    debug_str("\n=== TIMER TEST ===\n");
    debug_str("Waiting for timer ticks...\n");
    
    // Главный цикл - ждём тики
    uint64_t last_ticks = 0;
    while (1) {
        if (ticks != last_ticks) {
            last_ticks = ticks;
            debug_char('#');  // Каждый тик
        }
        asm volatile("pause");
    }
}