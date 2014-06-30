#ifndef CRT_H
#define CRT_H

typedef unsigned int uint32_t;

uint32_t
_syscall0(uint32_t number);

uint32_t
_syscall1(uint32_t number, uint32_t arg1);

void
regdump();

void
exit(int status);

void
yield();

uint32_t
fork();

uint32_t
wait();

int
main();

#endif
