#include "console.h"
#include "io.h"
#include "string.h"
#include "stdarg.h"

typedef enum {
    COLOUR_BLACK       = 0,
    COLOUR_BLUE        = 1,
    COLOUR_GREEN       = 2,
    COLOUR_CYAN        = 3,
    COLOUR_RED         = 4,
    COLOUR_PINK        = 5,
    COLOUR_BROWN       = 6,
    COLOUR_LIGHT_GREY  = 7,
    COLOUR_DARK_GREY   = 8,
    COLOUR_LIGHT_BLUE  = 9,
    COLOUR_LIGHT_GREEN = 10,
    COLOUR_LIGHT_CYAN  = 11,
    COLOUR_LIGHT_RED   = 12,
    COLOUR_LIGHT_PINK  = 13,
    COLOUR_YELLOW      = 14,
    COLOUR_WHITE       = 15,
}
colour_t;

static uint8_t
make_attr(colour_t background, colour_t foreground)
{
    return (background << 4) | foreground;
}

typedef struct {
    uint8_t character;
    uint8_t attr;
}
vchar_t;

static int x, y;

static const int width = 80, height = 25;

static vchar_t* const vram = (void*)0xb8000;

static uint16_t base_vga_port;

void
console_init()
{
    // read base vga port from bios data area
    base_vga_port = *(uint16_t*)0x463;

    memset16(vram, make_attr(COLOUR_BLACK, COLOUR_LIGHT_GREY) << 8, width * height);
    memset16(vram + width * height, (make_attr(COLOUR_RED,   COLOUR_WHITE) << 8) | 'X', width);
}

static void
scroll_up()
{
    memcpy(vram, vram + width, width * (height - 1) * 2);

    uint16_t empty_attr = make_attr(COLOUR_BLACK, COLOUR_LIGHT_GREY) << 8;
    memset16(vram + width * (height - 1), empty_attr, width);
}

static void
newline()
{
    x = 0;

    if(++y == height) {
        scroll_up();
        y = height - 1;
    }
}

static void
update_cursor()
{
    uint16_t pos = y * width + x;

    outb(base_vga_port, 0x0e);
    outb(base_vga_port + 1, (pos >> 8) & 0xff);

    outb(base_vga_port, 0x0f);
    outb(base_vga_port + 1, pos & 0xff);
}

static void
putc(char c)
{
    if(c == '\r') {
        x = 0;
        return;
    }

    if(c == '\n') {
        newline();
        return;
    }

    vram[y * width + x].attr = COLOUR_LIGHT_GREY;
    vram[y * width + x].character = c;

    if(++x == width) {
        newline();
    }
}

void
console_puts(const char* str)
{
    while(*str) {
        putc(*str++);
    }

    update_cursor();
}

static void
itoa(int n, char* out)
{
    int len = 0, negative = 0;

    if(n == 0) {
        out[len++] = '0';
        out[len++] = 0;
        return;
    }

    if(n < 0) {
        negative = 1;
        n *= -1;
    }

    while(n) {
        out[len++] = '0' + (n % 10);
        n /= 10;
    }

    if(negative) {
        out[len++] = '-';
    }

    out[len] = 0;

    for(int i = 0; i < len / 2; i++) {
        int j = len - i - 1;
        char c = out[j];
        out[j] = out[i];
        out[i] = c;
    }
}

static char
hex_dig(uint32_t dig)
{
    if(dig <= 9) {
        return '0' + dig;
    } else if(dig >= 10 && dig <= 15) {
        return 'a' + dig - 10;
    }
    return '?';
}

static void
utox(uint32_t u, char* out)
{
    out[0] = hex_dig((u >> 28) & 0xf);
    out[1] = hex_dig((u >> 24) & 0xf);
    out[2] = hex_dig((u >> 20) & 0xf);
    out[3] = hex_dig((u >> 16) & 0xf);
    out[4] = hex_dig((u >> 12) & 0xf);
    out[5] = hex_dig((u >>  8) & 0xf);
    out[6] = hex_dig((u >>  4) & 0xf);
    out[7] = hex_dig((u >>  0) & 0xf);
    out[8] = 0;
}

void
printf(const char* format, ...)
{
    va_list va;
    va_start(va, format);
    vprintf(format, va);
    va_end(va);
}

void
vprintf(const char* format, va_list va)
{
    while(1) {
        char c = *format++;

        if(c == 0) {
            break;
        }

        if(c != '%') {
            putc(c);
            continue;
        }

        switch(c = *format++) {
            case 'd': {
                char buff[16];
                itoa(va_arg(va, int), buff);
                console_puts(buff);
                break;
            }
            case 'x': {
                char buff[16];
                utox(va_arg(va, uint32_t), buff);
                console_puts(buff);
                break;
            }
            case 's': {
                console_puts(va_arg(va, const char*));
                break;
            }
            case 'c': {
                putc(va_arg(va, char));
                break;
            }
            case 0: {
                goto ret;
            }
            default: {
                putc(c);
                break;
            }
        }
    }
ret:
    update_cursor();
}
