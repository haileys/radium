#ifndef RADIUM_H
#define RADIUM_H

#define ENOSYS 1 // no such syscall
#define EFAULT 2 // bad address

// debug syscalls:
#define SYS_REGDUMP     0
#define SYS_CONSOLE_LOG 1

// process management syscalls:
#define SYS_EXIT        2
#define SYS_YIELD       3
#define SYS_FORK        4
#define SYS_WAIT        5

#endif
