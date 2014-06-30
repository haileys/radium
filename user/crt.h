#ifndef CRT_H
#define CRT_H

typedef unsigned int uint32_t;

uint32_t
_syscall0(uint32_t number);

uint32_t
_syscall1(uint32_t number, uint32_t arg1);

uint32_t
_syscall2(uint32_t number, uint32_t arg1, uint32_t arg2);

uint32_t
_syscall3(uint32_t number, uint32_t arg1, uint32_t arg2, uint32_t arg3);

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

void
console_log(const char* str);

int
main();

#endif
