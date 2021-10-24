#include "arenarray.h"

// Free data strucutre would use same quantity of pages
// so the total memory used may rach the double of this
// {0:8KB, 1:20KB, 2:40KB, 3:400KB, 4:4MB, 5:40MB, 6:400MB, 7:1GB, 8:1GB, 9:2.7GB}
uint32_t arena_mem_pages[ARENARRAY_LEN] = {2, 5, 10, 100, 1000, 10000, 100000, 270000, 270000, 700000};

ArenarrayItem ArenarrayItem_new(uint32_t bucket_size, uint8_t arenarray_index, uint32_t mem_pages) {
  return (ArenarrayItem){false, true, Arena_create(bucket_size, mem_pages, arenarray_index)};
}

Arenarray Arenarray_new(uint32_t bucket_size) {
  Arenarray arenarray = {bucket_size, {}};
  ArenarrayItem item = ArenarrayItem_new(bucket_size, 0, arena_mem_pages[0]);

  arenarray.arenas[0] = item;

  return arenarray;
}

Arena* Arenarray_arena_with_space(Arenarray* arenarray) {
  for(uint8_t i=0; i < ARENARRAY_LEN; i++) {
    ArenarrayItem arenarray_item = arenarray->arenas[i];
    if (arenarray_item.used && !arenarray_item.is_full) {
      return arenarray_item.arena;
    }
  }

  for(uint8_t i = 0; i < ARENARRAY_LEN; i++) {
    ArenarrayItem arenarray_item = arenarray->arenas[i];
    // TODO: Try arenarray_item->used instead
    if (!arenarray_item.used) {
      printf("Here for i %d \n", i);
      arenarray->arenas[i] = ArenarrayItem_new(arenarray->bucket_size, i, arena_mem_pages[i]);
      return arenarray->arenas[i].arena;
    }
  }

  return NULL;
}

MemBlock* Arenarray_find_mem_block(Arenarray* arenarray) {
    Arena* arena = Arenarray_arena_with_space(arenarray);

    // We ran out of space in the array
    if(!arena) {
        log_error("[Arenarray_get_memblock] No available space on Arenarray");
        return NULL;
    }

    MemBlock* memblock = Arena_get_mem_block(arena);
    /* printf("Memblock is here\n"); */

    if(memblock)  {
        return memblock;
    } else {
        arenarray->arenas[arena->header.arenarray_index].is_full = true;
        return Arenarray_find_mem_block(arenarray);
    }
}
