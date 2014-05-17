#ifndef STRING_H
#define STRING_H

#include "types.h"

void
memcpy(void* dest, void* src, size_t n);

void
memset(void* ptr, uint8_t c, size_t n);

void
memset16(void* ptr, uint16_t w, size_t n);

void
memset32(void* ptr, uint32_t l, size_t n);

bool
streq(const char* a, const char* b);

void
strlcpy(char* dest, const char* src, size_t dest_total_size);

#endif
