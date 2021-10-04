#pragma once

#include "common.h"
#include "free_stack.h"

typedef struct Arena Arena;

typedef struct ArenaHeader {
  size_t capacity;
  size_t len;
  uint16_t bucket_size;
  uint16_t pages;
  FreeStack* free_stack;
} ArenaHeader;

typedef struct Block {
  Arena* arena;
  char* block;
} Block;

typedef struct Arena {
  ArenaHeader* header;
  Arena* next_arena;
  Arena* prev_arena;
  Block blocks[];
} Arena;

Arena* Arena_create(uint16_t bucket_size);
bool Arena_destroy(Arena* arena);
