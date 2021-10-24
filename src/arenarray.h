#pragma once

#include "arena.h"
#include "common.h"

typedef struct ArenarrayItem {
  bool is_full;
  Arena* arena;
} ArenarrayItem;

#define ARENARRAY_LEN 10

ArenarrayItem ArenarrayItem_new(uint32_t bucket_size, uint8_t index,
                                uint32_t mem_pages);

typedef struct Arenarray {
  uint32_t bucket_size;
  ArenarrayItem arenas[ARENARRAY_LEN];
} Arenarray;

Arenarray Arenarray_new(uint32_t bucket_size);
Arena* Arenarray_arena_with_space(Arenarray* arenarray);
MemBlock* Arenarray_find_mem_block(Arenarray* arenarray);
bool Arenarray_free_memblock(Arenarray* arenarray, MemBlock* block,
                             uint8_t index);
bool Arenarray_remove_arena(Arenarray* arenarray, Arena* arena);
