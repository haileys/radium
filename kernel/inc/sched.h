#ifndef SCHED_H
#define SCHED_H

void
sched_begin() __attribute__((noreturn));

void
sched_switch();

#endif
