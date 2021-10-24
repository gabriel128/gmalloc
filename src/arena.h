#pragma once

#include "common.h"
#include "free_stack.h"

#define ARENA_LINEAR_GROWTH 2
#define FIRST_ARENA_PAGES_QTY 10

typedef struct Arena Arena;

typedef struct ArenaHeader {
  size_t capacity;
  size_t len;
  uint32_t bucket_size;
  uint32_t mem_pages;
  uint8_t arenarray_index;
} ArenaHeader;

typedef struct MemBlock {
  Arena* arena;
  byte data[];
} MemBlock;

typedef struct Arena {
  ArenaHeader header;
  FreeStack* free_stack;
  byte blocks[];
} Arena;

Arena* Arena_create(uint32_t, uint32_t, uint8_t);
bool Arena_destroy(Arena*);
MemBlock* Arena_get_mem_block(Arena*);
bool Arena_free_mem_block(MemBlock*);
