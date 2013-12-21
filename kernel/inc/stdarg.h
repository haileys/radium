#ifndef STDARG_H
#define STDARG_H

#include "types.h"

typedef uint32_t* va_list;

#define va_start(va, last_arg) ((va) = (va_list)&(last_arg))
#define va_arg(va, type) (*(type*)++va)
#define va_end(va) ((void)va)

#endif
