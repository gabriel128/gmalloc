#ifndef GMALLOC_H
#define GMALLOC_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <limits.h>

void gmalloc_init(int pages);
void* gmalloc(size_t size);
int gfree(void *ptr);
void gdump();


// Free Node
// Uses a double linked lists
// TODO: This data structure is terrible for cache locality, CHANGE!
typedef struct FreeNode {
    size_t size;
    struct FreeNode* next;
    struct FreeNode* prev;
    void* free_mem_region;
} FreeNode;

typedef struct AllocatedHeader {
    uint32_t size;
    uint32_t magic;
    AllocatedHeader* next;
} AllocatedHeader;

typedef struct AllocMetadata {
    // Global Mutex
    pthread_mutex_t lock;
    // Pointer where the first arena starts
    // TODO: This will be a collection of Arenas at some point
    void* arena_init;
    // Head of the FreeNode data structure
    FreeNode* free_node_head;
    // Header of the first memory allocation
    // this should be the same as arena_init
    // TODO: This  will be a collection of heads
    AllocatedHeader* alloc_head;
    // Size of the arena, needed for checking if
    // the Arena is running out of space
    size_t arena_size;
} AllocMetadata;

#define MAGIC 0x1234567

static inline bool free_node_is_head(FreeNode* free_node, AllocMetadata metadata) {
   return free_node == metadata.free_node_head;
}

#endif /* GMALLOC_H */

