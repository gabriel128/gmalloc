#include "arena.h"

static size_t calculate_capacity(char* init, uint16_t pages, char* block_init, uint16_t block_size) {
    size_t end_of_arena =  (size_t) (init + PAGE_SIZE * pages);
    return (size_t)((end_of_arena - (size_t)block_init) / block_size);
}

Arena* Arena_create(uint16_t bucket_size) {
  // TODO make it dynamic
  uint16_t pages = 1;

  ArenaHeader* arena_header = mem_zero_init(pages);

  arena_header->bucket_size = bucket_size;
  arena_header->len = 0;
  arena_header->pages = pages;
  arena_header->free_stack = FreeStack_new(1);

  Arena* arena = (Arena*)(arena_header + 1);
  arena->header = arena_header;

  arena_header->capacity = calculate_capacity((char*)arena_header, 1, (char*)arena->blocks, bucket_size);

  log_debug(
      "[Arena_create] header is at %p, blocks is at %p, arena is at %p \n",
      arena_header, arena->blocks, arena);

  return arena;
}

bool Arena_destroy(Arena* arena) {
  bool free_stack_freed = FreeStack_destroy(arena->header->free_stack);
  int err = munmap(arena->header, arena->header->pages);

  if (err != 0 || !free_stack_freed) {
    log_error("FreeStack unmapping failed \n");
    return false;
  } else {
    return true;
  }
}
