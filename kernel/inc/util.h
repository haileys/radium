#ifndef UTIL_H
#define UTIL_H

#include "types.h"

#define static_assert(name, expr) static char __static_assert__##name[(expr) ? 1 : -1] __attribute__((unused))

static inline size_t
round_down(size_t val, size_t divisor)
{
    return val - val % divisor;
}

static inline size_t
round_up(size_t val, size_t divisor)
{
    return round_down(val + divisor - 1, divisor);
}

#endif
