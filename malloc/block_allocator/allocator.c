#include "allocator.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "utils.h"

struct blk_allocator *blka_new(void)
{
    struct blk_allocator *new = malloc(sizeof(struct blk_allocator));
    new->meta = NULL;
    return new;
}

struct blk_meta *blka_alloc(struct blk_allocator *blka, size_t size)
{
    int p_size = sysconf(_SC_PAGE_SIZE);
    int s = size / p_size + 1;
    if (!blka->meta)
    {
        blka->meta = mmap(NULL, p_size * s, PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        blka->meta->size =
            s * p_size - sizeof(struct blka_meta *) - sizeof(size_t);
    }
    else
    {
        struct blk_meta *new = mmap(NULL, p_size * s, PROT_READ | PROT_WRITE,
                                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        new->size = s *p_size - sizeof(struct blka_meta *) - sizeof(size_t);
        new->next = blka->meta;
        blka->meta = new;
    }
    return blka->meta;
}

void blka_free(struct blk_meta *blk)
{
    munmap(blk, blk->size);
}

void blka_pop(struct blk_allocator *blka)
{
    if (!blka->meta)
    {
        return;
    }
    struct blk_meta *tmp = blka->meta;
    blka->meta = blka->meta->next;
    blka_free(tmp);
}

void blka_delete(struct blk_allocator *blka)
{
    while (blka->meta)
    {
        blka_pop(blka);
    }
    free(blka);
}
