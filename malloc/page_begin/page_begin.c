#include "stddef.h"

void *page_begin(void *ptr, size_t page_size)
{
    char *new = NULL;
    new += (size_t)ptr & ~(page_size - 1);
    return new;
}
