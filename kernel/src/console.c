#include <console.h>
#include <string.h>

enum {
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
};

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
    memset(vram, 0, width * height * 2);
}

static void
scroll_up()
{
    memcpy(vram + width * 2, vram, width * height * 2);
    memset(vram + width * 2 * (height - 1), 0, width * 2);
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
}

void
printf(const char* format, ...)
{
    (void)format;
    // TODO
}
