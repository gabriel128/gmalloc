#pragma once

#include "common.h"
#include "arena.h"

typedef struct ArenarrayItem {
  bool is_full;
  bool used;
  Arena* arena;
} ArenarrayItem;

#define ARENARRAY_LEN 10

ArenarrayItem ArenarrayItem_new(uint32_t bucket_size, uint8_t index, uint32_t mem_pages);

typedef struct Arenarray {
  uint32_t bucket_size;
  ArenarrayItem arenas[ARENARRAY_LEN];
} Arenarray;

Arenarray Arenarray_new(uint32_t bucket_size);
Arena* Arenarray_arena_with_space(Arenarray* arenarray);
MemBlock* Arenarray_find_mem_block(Arenarray* arenarray);
MemBlock* Arenarray_free_memblock(Arenarray* arenarray, Arena* arena);
