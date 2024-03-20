#include "alignment.h"

#include <stddef.h>
//#include <stdlib.h>

size_t align(size_t size)
{
    if (size == 0)
        return 0;
    int overflow = 0;
    if (size % sizeof(long double))
        overflow = 1 + size / sizeof(long double);
    else
        overflow = size / sizeof(long double);
    long unsigned int res = 0;
    if (__builtin_umull_overflow(overflow, sizeof(long double), &res))
    {
        return 0;
    }
    return res;
}
