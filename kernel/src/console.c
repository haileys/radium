#include <console.h>
#include <io.h>
#include <string.h>

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

void
console_init()
{
    memset16(vram, make_attr(COLOUR_BLACK, COLOUR_LIGHT_GREY) << 8, width * height);
}

static void
scroll_up()
{
    memcpy(vram + width * 2, vram, width * height * 2);

    uint16_t empty_attr = make_attr(COLOUR_BLACK, COLOUR_LIGHT_GREY) << 8;
    memset16(vram + width * 2 * (height - 1), empty_attr, width);
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
    // read base vga port from bios data area
    uint16_t base_vga_port = *(uint16_t*)0x463;

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

void
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

void
printf(const char* format, ...)
{
    uint32_t* va = (uint32_t*)&format + 1;
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
                int n = *(int*)va++;
                itoa(n, buff);
                console_puts(buff);
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
