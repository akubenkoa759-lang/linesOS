#include <stdint.h>

volatile uint16_t* vga = (volatile uint16_t*)0xB8000;

#define DARK_BG    0x0F
#define PANEL_BG   0x1F
#define ACCENT_TXT 0x0B
#define BAR_FILL   0x3B
#define BAR_EMPTY  0x80
#define DESKTOP_BG 0x17

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void wait_for_enter(void) {
    while (1) {
        if (inb(0x64) & 1) {
            uint8_t scancode = inb(0x60);
            if (scancode == 0x1C) {
                break;
            }
        }
    }
}

void delay(uint32_t count) {
    for (volatile uint32_t i = 0; i < count * 100000; i++) {
        __asm__ volatile ("nop");
    }
}

void clear_screen(uint8_t color) {
    for (int i = 0; i < 80 * 25; i++) {
        vga[i] = (uint16_t)' ' | ((uint16_t)color << 8);
    }
}

void print_at(const char* str, int x, int y, uint8_t color) {
    int pos = y * 80 + x;
    for (int i = 0; str[i] != '\0'; i++) {
        vga[pos + i] = (uint16_t)str[i] | ((uint16_t)color << 8);
    }
}

void draw_box(int x, int y, int w, int h, uint8_t color) {
    for (int row = y; row < y + h; row++) {
        for (int col = x; col < x + w; col++) {
            vga[row * 80 + col] = (uint16_t)' ' | ((uint16_t)color << 8);
        }
    }
}

void kernel_main(void) {
    // 1. Приветствие собственного загрузчика и установщика
    clear_screen(DARK_BG);
    draw_box(0, 0, 80, 1, PANEL_BG);
    print_at(" linesOS 1.0 (100% Custom Boot & Kernel | 2026 Edition)", 1, 0, PANEL_BG);

    draw_box(12, 5, 56, 14, 0x07);
    print_at("========================================================", 12, 5, ACCENT_TXT);
    print_at("          DOBRO POZHALOVAT V linesOS 1.0                ", 12, 7, 0x0F);
    print_at("========================================================", 12, 8, ACCENT_TXT);
    
    print_at("Yadro: linesKernel v1.0 (Napisano s nulya)", 15, 10, 0x07);
    print_at("Zagruzchik: Custom 512-byte Bootloader (Bez GRUB)", 15, 11, 0x07);
    
    print_at(">>> Nazhmite [ ENTER ] dlya nachala ustanovki <<<", 15, 15, 0x0E);

    wait_for_enter();

    // 2. Виртуальная установка в оперативной памяти
    draw_box(15, 14, 50, 3, 0x07);
    print_at("Ustanovka komponentov linesOS 1.0...", 15, 13, 0x0F);

    int bar_x = 15;
    int bar_y = 15;
    int bar_width = 50;

    draw_box(bar_x, bar_y, bar_width, 1, BAR_EMPTY);

    for (int i = 0; i <= bar_width; i++) {
        vga[bar_y * 80 + (bar_x + i)] = (uint16_t)' ' | ((uint16_t)BAR_FILL << 8);

        if (i == 0)  print_at("Modul: Zapusk yadra linesKernel...               ", 15, 17, ACCENT_TXT);
        if (i == 12) print_at("Modul: Initsializatsiya VGA i klaviatury...      ", 15, 17, ACCENT_TXT);
        if (i == 25) print_at("Modul: Nastroyka sredy vypolneniya 2026...       ", 15, 17, ACCENT_TXT);
        if (i == 38) print_at("Modul: Proverka tselostnosti pamyati...          ", 15, 17, ACCENT_TXT);
        if (i == 47) print_at("Modul: Zavershenie ustanovki...                  ", 15, 17, ACCENT_TXT);

        delay(110);
    }

    print_at("GOTOVO! linesOS 1.0 uspeshno ustanovlena!        ", 15, 17, 0x0A);
    delay(900);

    // 3. Рабочий стол linesOS 1.0
    clear_screen(DESKTOP_BG);
    
    draw_box(0, 0, 80, 1, 0x0F);
    print_at(" [ linesOS 1.0 ] ", 0, 0, 0x30);
    print_at("Kernel: Custom | Arch: x86 | 2026", 46, 0, 0x0F);

    draw_box(18, 8, 44, 8, 0x07);
    print_at("--------------------------------------------", 18, 8, ACCENT_TXT);
    print_at("           linesOS 1.0 (2026)               ", 18, 10, 0x0F);
    print_at("   Polnostyu avtonomnaya OS s nulya!        ", 18, 12, 0x07);
    print_at("--------------------------------------------", 18, 15, ACCENT_TXT);
}
