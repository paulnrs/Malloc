#include <stddef.h>

void *beware_overflow(void *ptr, size_t nmemb, size_t size)
{
    size_t res = nmemb * size;
    if (__builtin_umull_overflow(nmemb, size, &res))
    {
        return NULL;
    }
    char *res1 = ptr;
    return res1 + res;
}
