#include "arena.h"

static size_t calculate_capacity(byte* init, uint32_t pages, byte* block_init, uint32_t block_size) {
    size_t end_of_arena =  (size_t) (init + PAGE_SIZE * pages);
    return (size_t)((end_of_arena - (size_t)block_init) / block_size);
}

Arena* Arena_create(uint32_t bucket_size, uint32_t mem_pages) {
  Arena* arena = mem_zero_init(mem_pages);
  arena->free_stack = FreeStack_new(1);

  ArenaHeader* arena_header = &arena->header;
  arena_header->bucket_size = bucket_size;
  arena_header->len = 0;
  arena_header->mem_pages = mem_pages;

  arena_header->capacity = calculate_capacity((byte*)arena_header, 1, (byte*)arena->blocks, bucket_size);

  log_debug(
      "[Arena_create] header is at %p, blocks is at %p, arena is at %p \n",
      arena_header, arena->blocks, arena);

  return arena;
}

bool Arena_destroy(Arena* arena) {
  bool free_stack_freed = FreeStack_destroy(arena->free_stack);
  int err = munmap(arena, arena->header.mem_pages);

  if (err != 0 || !free_stack_freed) {
    log_error("FreeStack unmapping failed \n");
    return false;
  } else {
    return true;
  }
}
