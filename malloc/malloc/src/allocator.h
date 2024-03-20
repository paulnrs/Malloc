#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

struct free_list
{
    struct free_list *next; // next free block
};

struct blk_meta
{
    int is_free;
    size_t size;
    struct blk_meta *next;
    char *data;
};

struct blk_allocator
{
    void *free; // address of the first free block of the free list
    struct blk_meta *meta;
    size_t capacity;
    struct blk_allocator *next;
};

void *my_malloc(size_t size);
void my_free(void *ptr);
struct blk_allocator *blka_new(size_t size);
void print_memory(void);
void *blka_alloc(struct blk_allocator *blka, size_t size);
void blka_free(struct blk_meta *ptr);
void blka_free2(struct blk_meta *ptr);
void *my_realloc(void *ptr, size_t size);
size_t align(size_t size, size_t size2);
void *my_calloc(size_t nmemb, size_t size);
void my_free2(void *ptr);

#endif /* !ALLOCATOR_H */
