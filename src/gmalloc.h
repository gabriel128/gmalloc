#pragma once

#include "common.h"
#include "free_stack.h"
// Assumed 4kb pages for simplicity
#define PAGE_SIZE 4096

void gmalloc_init(int pages);
void *gmalloc(size_t size);
int gfree(void *ptr);
void gdump();

typedef struct ArenaHeader {
  uint16_t bucket_size;
  size_t size_bytes;
  size_t capacity;
  size_t len;
  // Points to the next available slot
  size_t cursor;
} ArenaHeader;

typedef struct Arena {
  ArenaHeader header;
  FreeStack *free_stack;
  void *arena_start_ptr;
} Arena;

typedef struct GMallocMetadata {
  pthread_mutex_t lock;
  size_t arena_size;
  void *arena_init;
  Arena arena;
} GMAllocMetadata;
