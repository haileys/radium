#include "string.h"

void
memcpy(void* dest_, void* src_, size_t n)
{
    char* dest = dest_;
    char* src = src_;

    if(src < dest) {
        while(n--) {
            *dest++ = *src++;
        }
    } else {
        dest += n;
        src += n;
        while(n--) {
            *--dest = *--src;
        }
    }
}

void
memset(void* ptr_, uint8_t c, size_t n)
{
    uint8_t* ptr = ptr_;
    while(n--) {
        *ptr++ = c;
    }
}

void
memset16(void* ptr_, uint16_t w, size_t n)
{
    uint16_t* ptr = ptr_;
    while(n--) {
        *ptr++ = w;
    }
}
