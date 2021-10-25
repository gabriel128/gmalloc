#pragma once

#include "common.h"
#include "free_stack.h"

#define ARENA_LINEAR_GROWTH 2
#define FIRST_ARENA_PAGES_QTY 10

typedef struct Arena Arena;

typedef struct ArenaHeader {
  FreeStack* free_stack;
  size_t capacity;
  size_t len;
  uint32_t bucket_size;
  uint32_t mem_pages;
  uint8_t arenarray_index;
} ArenaHeader;

typedef struct MemBlockCache {
  uint32_t bucket_size;
  uint8_t arenarray_index;
} MemBlockCache;

typedef struct MemBlock {
  ArenaHeader* arena_header;
  byte data[];
} MemBlock;

typedef struct Arena {
  ArenaHeader header;
  FreeStack* free_stack;
  byte blocks[];
} Arena;

typedef enum { FREE_STACK_HAS_SPACE, FREE_STACK_FULL, UNKOWN } FreeStackState;

typedef struct FreeResult {
  bool success;
  FreeStackState free_stack_state;
} FreeResult;

Arena* Arena_create(uint32_t, uint32_t, uint8_t);
bool Arena_destroy(Arena*);
MemBlock* Arena_get_mem_block(Arena*);
FreeResult Arena_free_mem_block(MemBlock* block);
