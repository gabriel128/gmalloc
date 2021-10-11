#include "arena.h"

Arena* Arena_create(uint32_t bucket_size, uint32_t mem_pages) {
  if (bucket_size > (PAGE_SIZE/2 + 1)) {
    log_error("[Arena_create] can't allocate more than %d", PAGE_SIZE/2 + 1);
    perror("Max allowed size\n");
    return NULL;
  }

  if (bucket_size == 0 || mem_pages == 0) {
    return NULL;
  }

  Arena* arena = mem_zero_init(mem_pages);
  arena->free_stack = FreeStack_new(mem_pages);

  ArenaHeader* arena_header = &arena->header;
  arena_header->bucket_size = bucket_size;
  arena_header->len = 0;
  arena_header->mem_pages = mem_pages;

  arena_header->capacity =
      calculate_capacity((byte*)arena_header, mem_pages, arena->blocks,
                         sizeof(Arena*) + bucket_size);

  log_debug(
      "[Arena_create] header is at %p, blocks starts at %p, arena is at %p \n",
      arena_header, arena->blocks, arena);

  if (arena_header->capacity > arena->free_stack->capacity) {
    log_error("[Arena_create] Arena capacity is bigger than the free_stack can "
              "support");
    return NULL;
  }

  return arena;
}

bool Arena_destroy(Arena* arena) {
  if (arena == NULL) {
    return false;
  }

  Arena* prev_arena = arena->prev_arena;
  Arena* next_arena = arena->next_arena;

  if (prev_arena != NULL) {
    prev_arena->next_arena = next_arena;
  }

  if (next_arena != NULL) {
    next_arena->prev_arena = prev_arena;
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
  if(free_stack->len == 0) return NULL;

  PtrResult mem_block_res = FreeStack_pop(free_stack);

  if (IS_ERR(mem_block_res)) {
    log_error("[Arena_get_mem_block] Error on poping free_stack");
    return NULL;
  }

  return (MemBlock*)mem_block_res.the.val;
}

MemBlock* Arena_get_mem_block(Arena* arena) {
  if (arena == NULL) {
    return NULL;
  }

  MemBlock* mem_block = try_from_free_stack(arena->free_stack);

  if (mem_block != NULL)
    return mem_block;

  ArenaHeader* header = &arena->header;

  if (header->len < header->capacity) {

    MemBlock* block = (MemBlock*)next_available_block_position(arena);
    block->arena = arena;

    header->len++;

    return block;
  }

  Arena* next_arena = arena->next_arena;

  if (next_arena != NULL) {
    return Arena_get_mem_block(next_arena);
  } else {
    // Create a new Arena
    uint32_t next_mem_pages_size = header->mem_pages * ARENA_LINEAR_GROWTH;
    Arena* new_arena = Arena_create(header->bucket_size, next_mem_pages_size);
    arena->next_arena = new_arena;
    new_arena->prev_arena = arena;
    return Arena_get_mem_block(new_arena);
  }
}

inline bool Arena_is_head(Arena* arena) { return arena->prev_arena == NULL; }

// NOTE: Double freeing a memory block is undefined behaviour
// (for now)
bool Arena_free_mem_block(MemBlock* block) {
  if (block == NULL) {
    return false;
  }

  Arena* arena = block->arena;
  FreeStack* free_stack = arena->free_stack;

  bool all_arena_blocks_freed = (free_stack->len + 1) == arena->header.len;

  // We always keep the head alive
  if (!Arena_is_head(arena) && all_arena_blocks_freed) {
    return Arena_destroy(arena);
  } else {
    return FreeStack_push(free_stack, (byte*)block);
  }
}
