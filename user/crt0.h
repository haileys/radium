#ifndef CRT0_H
#define CRT0_H

typedef unsigned int uint32_t;

void
regdump();

void
exit(int status);

void
yield();

uint32_t
fork();

int
main();

#endif
