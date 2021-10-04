#pragma once

#include "common.h"
#include "free_stack.h"
// Assumed 4kb pages for simplicity
#define PAGE_SIZE 4096

typedef struct ArenaHeader {
  size_t size_in_bytes;
  size_t capacity;
  size_t len;
  uint16_t bucket_size;
  FreeStack* free_stack;
} ArenaHeader;

typedef struct Arena {
  ArenaHeader* header;
  /* char* arena_start_ptr; */
  // Points to the next available slot
  byte tail[];
} Arena;

typedef struct GMallocMetadata {
  Arena* arenas[7];
  bool arenas_created[7];
} GMAllocMetadata;

void gmalloc_init(int pages);
void* gmalloc(size_t size);
int gfree(void* ptr);
void gdump();
GMAllocMetadata* gmalloc_metadata();
