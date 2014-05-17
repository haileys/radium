#include "string.h"

void
memcpy(void* dest_, void* src_, size_t n)
{
    char* dest = dest_;
    char* src = src_;

    if(src > dest) {
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

void
memset32(void* ptr_, uint32_t l, size_t n)
{
    uint32_t* ptr = ptr_;
    while(n--) {
        *ptr++ = l;
    }
}

bool
streq(const char* a, const char* b)
{
    while(1) {
        if(*a != *b) {
            return false;
        }
        if(*a == 0) {
            return true;
        }
        a++;
        b++;
    }
}

void
strlcpy(char* dest, const char* src, size_t dest_total_size)
{
    if(dest_total_size == 0) {
        return;
    }

    while(*src && dest_total_size > 1) {
        *dest++ = *src++;
    }

    *dest = 0;
}
