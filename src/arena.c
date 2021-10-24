#include "arena.h"

#define ARENAS_QTY 100000

Arena* Arena_create(uint32_t bucket_size, uint32_t mem_pages,
                    uint8_t arenarray_index) {
  if (bucket_size > (PAGE_SIZE / 2 + 1)) {
    log_error("[Arena_create] can't allocate more than %d", PAGE_SIZE / 2 + 1);
    perror("Max allowed size\n");
    return NULL;
  }

  if (bucket_size == 0 || mem_pages == 0) {
    return NULL;
  }

  Arena* arena = mem_zero_init(mem_pages);
  arena->free_stack = NULL;

  ArenaHeader* arena_header = &arena->header;
  arena_header->arenarray_index = arenarray_index;
  arena_header->bucket_size = bucket_size;
  arena_header->len = 0;
  arena_header->mem_pages = mem_pages;

  arena_header->capacity =
      calculate_capacity((byte*)arena_header, mem_pages, arena->blocks,
                         sizeof(Arena*) + bucket_size);

  log_debug(
      "[Arena_create] header is at %p, blocks starts at %p, arena is at %p \n",
      arena_header, arena->blocks, arena);

  return arena;
}

bool Arena_destroy(Arena* arena) {
  if (arena == NULL) {
    return false;
  }
  bool free_stack_freed = FreeStack_destroy(arena->free_stack);
  int err = munmap(arena, arena->header.mem_pages);

  if (err != 0 || !free_stack_freed) {
    log_error("FreeStack unmapping failed \n");
    return false;
  } else {
    return true;
  }
}

void* next_available_block_position(Arena* arena) {
  ArenaHeader header = arena->header;
  return (arena->blocks + (sizeof(MemBlock) + header.bucket_size) * header.len);
}

static MemBlock* try_from_free_stack(FreeStack* free_stack) {
  if (free_stack->len == 0)
    return NULL;

  PtrResult mem_block_res = FreeStack_pop(free_stack);

  if (IS_ERR(mem_block_res)) {
    log_error("[Arena_get_mem_block] Error on poping free_stack");
    return NULL;
  }

  return (MemBlock*)mem_block_res.the.val;
}

// Returns NULL when no more mem_block availables
MemBlock* Arena_get_mem_block(Arena* arena) {
  if (arena->free_stack != NULL) {
    MemBlock* mem_block = try_from_free_stack(arena->free_stack);

    if (mem_block) {
      return mem_block;
    }
  }

  ArenaHeader* header = &arena->header;

  if (header->len < header->capacity) {
    MemBlock* block = (MemBlock*)next_available_block_position(arena);
    block->arena = arena;

    header->len++;

    return block;
  } else {
    return NULL;
  }
}

// NOTE: Double freeing a memory block is undefined behaviour
// (for now)
// It returns true if the memory has been added to the stack
// or false if the stack is full
FreeResult Arena_free_mem_block(MemBlock* block) {
  if (!block) {
    return (FreeResult){false, UNKOWN};
  }

  Arena* arena = block->arena;

  if (!arena->free_stack) {
    arena->free_stack = FreeStack_new(arena->header.mem_pages);
  }

  FreeStack* free_stack = arena->free_stack;

  if (free_stack->len >= arena->header.len) {
    return (FreeResult){false, FREE_STACK_FULL};
  } else {
    bool ok = FreeStack_push(free_stack, (byte*)block);
    FreeStackState free_stack_state = free_stack->len == arena->header.len
                                          ? FREE_STACK_FULL
                                          : FREE_STACK_HAS_SPACE;

    return (FreeResult){ok, free_stack_state};
  }
}
