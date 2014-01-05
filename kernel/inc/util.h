#ifndef UTIL_H
#define UTIL_H

#include "types.h"

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
