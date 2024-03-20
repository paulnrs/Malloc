#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "allocator.h"

struct blk_allocator *blka = NULL;

size_t align(size_t size, size_t size2)
{
    if (size == 0)
        return 0;
    int overflow = 0;
    if (size % size2)
        overflow = 1 + size / size2;
    else
        overflow = size / size2;
    long unsigned int res = 0;
    if (__builtin_umull_overflow(overflow, size2, &res))
    {
        return 0;
    }
    return res;
}

void *my_malloc(size_t size)
{
    if (!blka)
    {
        blka = blka_new(size);
    }
    void *new = blka_alloc(blka, size);
    return new;
}

void my_free(void *ptr)
{
    struct blk_meta *to_free = ptr;
    blka_free(to_free);
}

void *my_realloc(void *ptr, size_t size)
{
    size = align(size, sizeof(long double));
    char *str1 = ptr;
    str1 -= 32;
    void *tmp = str1;
    struct blk_meta *str = tmp;
    if (str->size < size)
    {
        void *new = my_malloc(size);
        memcpy(new, ptr, str->size);
        my_free(ptr);
        return new;
    }
    else
    {
        str->size = size;
    }
    return ptr;
}
void *my_calloc(size_t nmemb, size_t size)
{
    size_t i = 0;
    if (__builtin_umull_overflow(nmemb, size, &i))
    {
        return NULL;
    }
    void *new = my_malloc(size * nmemb);
    if (!new)
        return NULL;
    memset(new, 0, nmemb * size);
    return new;
}

__attribute__((visibility("default"))) void *malloc(size_t size)
{
    return my_malloc(size);
}

__attribute__((visibility("default"))) void free(void *ptr)
{
    if (!ptr)
        return;
    my_free(ptr);
    return;
}

__attribute__((visibility("default"))) void *realloc(void *ptr, size_t size)
{
    if (!ptr)
        return my_malloc(size);
    if (!size)
    {
        my_free(ptr);
        return NULL;
    }
    return my_realloc(ptr, size);
}

__attribute__((visibility("default"))) void *calloc(size_t nmemb, size_t size)
{
    return my_calloc(nmemb, size);
}
