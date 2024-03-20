#include "allocator.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

extern struct blk_allocator *blka;

#define STRUCT 32

struct blk_allocator *blka_new(size_t size)
{
    size_t p_size = sysconf(_SC_PAGE_SIZE);
    size = align(size + sizeof(struct blk_allocator) + STRUCT, p_size);
    struct blk_allocator *new = mmap(NULL, size, PROT_READ | PROT_WRITE,
                                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    new->capacity = size - sizeof(struct blk_allocator);
    void *shift = new + 1;
    struct blk_meta *meta = shift;
    new->meta = meta;
    new->next = NULL;
    meta->next = NULL;
    meta->is_free = 1;
    meta->size = size - sizeof(struct blk_allocator);
    return new;
}

void *blka_alloc(struct blk_allocator *blka, size_t size)
{
    size = align(size, sizeof(long double));
    struct blk_meta *tmp = blka->meta;
    while (blka)
    {
        int found = 0;
        tmp = blka->meta;
        while (tmp)
        {
            if (tmp->is_free == 1 && tmp->size > size + STRUCT + 32)
            {
                found = 1;
                break;
            }
            tmp = tmp->next;
        }
        if (found)
            break;
        else if (blka->next == NULL)
            blka->next = blka_new(size);
        blka = blka->next;
    }
    size_t full_size = tmp->size;
    tmp->size = size;
    tmp->is_free = 0;
    void *void1 = tmp;
    char *shift1 = void1;
    shift1 = shift1 + STRUCT + size;
    void *void2 = shift1;
    void *res = tmp + 1;
    struct blk_meta *meta = void2;
    meta->size = full_size - size - STRUCT;
    meta->is_free = 1;
    meta->next = tmp->next;
    tmp->next = meta;
    return res;
}

static void handle_nonext(void)
{
    struct blk_allocator *page = blka;
    if (!page->next)
    {
        int is_clear = 1;
        struct blk_meta *meta = page->meta;
        while (meta)
        {
            if (meta->is_free == 0)
            {
                is_clear = 0;
                break;
            }
            meta = meta->next;
        }
        if (is_clear)
        {
            munmap(blka, blka->capacity);
            blka = NULL;
        }
    }
}

static int handle_next(void)
{
    int is_clear = 1;
    struct blk_meta *meta = blka->meta;
    while (meta)
    {
        if (meta->is_free == 0)
        {
            is_clear = 0;
            break;
        }
        meta = meta->next;
    }
    if (is_clear)
    {
        struct blk_allocator *tmp = blka->next;
        munmap(blka, blka->capacity);
        blka = tmp;
        return 1;
    }
    return 0;
}

static void is_free(void)
{
    int last = 0;
    struct blk_allocator *page = blka;
    if (!page->next)
    {
        handle_nonext();
    }
    else
    {
        if (handle_next())
            return;
        struct blk_allocator *page_n = blka->next;
        while (page_n)
        {
            int is_clear = 1;
            struct blk_meta *meta = page_n->meta;
            while (meta)
            {
                if (meta->is_free == 0)
                {
                    is_clear = 0;
                    break;
                }
                meta = meta->next;
            }
            if (is_clear)
            {
                page->next = page_n->next;
                munmap(page_n, page_n->capacity);
                last = 1;
            }
            if (last)
                break;
            page = page->next;
            page_n = page_n->next;
        }
    }
}

void blka_free(struct blk_meta *ptr)
{
    ptr -= 1;
    ptr->is_free = 1;
    is_free();
}
