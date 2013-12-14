#ifndef IO_H
#define IO_H

#include "types.h"

static void __attribute__((unused))
outb(uint16_t port, uint8_t value)
{
    __asm__ volatile("outb %1, %0" :: "r"(value), "Nd"(port));
}

static uint8_t __attribute__((unused))
inb(uint16_t port)
{
    volatile uint8_t value;
    __asm__ volatile("inb %0, %1" : "=r"(value) : "Nd"(port));
    return value;
}

static void __attribute__((unused))
outl(uint16_t port, uint32_t value)
{
    __asm__ volatile("outd %1, %0" :: "r"(value), "Nd"(port));
}

static uint32_t __attribute__((unused))
inl(uint16_t port)
{
    volatile uint32_t value;
    __asm__ volatile("ind %0, %1" : "=r"(value) : "Nd"(port));
    return value;
}

#endif
