#include "recycler.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct recycler *recycler_create(size_t block_size, size_t total_size)
{
    if ((block_size % sizeof(size_t)) != 0 || block_size == 0 || total_size == 0
        || total_size % block_size != 0)
    {
        return NULL;
    }
    struct recycler *new = malloc(sizeof(struct recycler));
    void *chunk = malloc(total_size);
    if (!new || !chunk)
    {
        return NULL;
    }
    new->block_size = block_size;
    new->capacity = total_size / block_size;
    new->chunk = chunk;
    new->free = new->chunk;
    struct free_list *tmp = new->free;
    for (size_t i = 1; i < new->capacity; i++)
    {
        tmp->next = tmp + (block_size / 8);
        tmp = tmp->next;
    }
    return new;
}

void recycler_destroy(struct recycler *r)
{
    if (!r)
    {
        return;
    }
    free(r->chunk);
    free(r);
}

void *recycler_allocate(struct recycler *r)
{
    if (!r || !(r->free))
    {
        return NULL;
    }
    void *res = r->free;
    struct free_list *tmp = r->free;
    tmp = tmp->next;
    r->free = tmp;
    return res;
}

void recycler_free(struct recycler *r, void *block)
{
    if (!r || !block)
    {
        return;
    }
    struct free_list *free1 = block;
    free1->next = r->free;
    r->free = free1;
}
