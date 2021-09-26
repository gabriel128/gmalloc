#pragma once

#include "common.h"
#include "free_stack.h"
// Assumed 4kb pages for simplicity
#define PAGE_SIZE 4096

void gmalloc_init(int pages);
void* gmalloc(size_t size);
int gfree(void* ptr);
void gdump();

typedef struct ArenaHeader {
  uint16_t bucket_size;
  size_t size_in_bytes;
  size_t capacity;
  size_t len;
} ArenaHeader;

typedef struct Arena {
  ArenaHeader header;
  FreeStack* free_stack;
  char* arena_start_ptr;
  // Points to the next available slot
  char* tail;
} Arena;

typedef struct GMallocMetadata {
  Arena arenas[7];
  bool arenas_created[7];
} GMAllocMetadata;
