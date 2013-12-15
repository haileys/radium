#ifndef PANIC_H
#define PANIC_H

#include <multiboot.h>

void
panic(const char* message) __attribute__((noreturn));

#endif
