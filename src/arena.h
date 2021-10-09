#pragma once

#include "common.h"
#include "free_stack.h"

typedef struct Arena Arena;

typedef struct ArenaHeader {
  size_t capacity;
  size_t len;
  uint32_t bucket_size;
  uint32_t mem_pages;
} ArenaHeader;

typedef struct MemBlock {
  Arena* arena;
  byte data[];
} MemBlock;

typedef struct Arena {
  ArenaHeader header;
  FreeStack* free_stack;
  Arena* next_arena;
  Arena* prev_arena;
  byte blocks[];
} Arena;


Arena* Arena_create(uint32_t, uint32_t);
bool Arena_is_head(Arena*);
bool Arena_destroy(Arena*);
MemBlock* Arena_push_mem_block(Arena*);
bool Arena_free_mem_block(MemBlock*);
