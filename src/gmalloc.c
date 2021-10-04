#include "gmalloc.h"

thread_local GMAllocMetadata metadata;

bool has_been_initialized = false;

size_t find_bucket_index(size_t size) {
  if (0 < size && size <= 8)
    return 0;

  if (8 < size && size <= 16)
    return 1;

  if (16 < size && size <= 32)
    return 2;

  if (32 < size && size <= 64)
    return 3;

  if (64 < size && size <= 128)
    return 4;

  if (128 < size && size <= 256)
    return 5;

  if (256 < size && size <= 512)
    return 6;

  return -1;
}

size_t bucket_size_from_index(size_t index) { return 1 << (index + 3); }

void* mem_init(int pages) {
  int fd = open("/dev/zero", O_RDWR);

  int page_size = PAGE_SIZE;

  // Init arena
  void* arena_init =
      mmap(NULL, page_size * pages, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

  if (arena_init == MAP_FAILED) {
    perror("Error on mmap");
    exit(1);
  }
  close(fd);

  return arena_init;
}

static Arena* create_arena(uint16_t bucket_size) {
  // TODO make it dynamic
  uint32_t pages = 1;
  ArenaHeader* arena_header = mem_init(pages);

  arena_header->bucket_size = bucket_size,
  arena_header->size_in_bytes = PAGE_SIZE * pages,
  // TODO: calculate capacity as: capacity = (arena_header + PAGE_SIZE -
  // tail_addr) / bucket_size
      arena_header->capacity = PAGE_SIZE * pages / bucket_size,
  arena_header->free_stack = FreeStack_new(1);
  arena_header->len = 0;

  Arena* arena = (Arena*)(arena_header + 1);
  arena->header = arena_header;

  log_debug("[create_arena] header is at %p, tail is at %p, arena is at %p \n",
            arena_header, arena->tail, arena);

  return arena;
}

static void destroy_arena(Arena* arena) {
  FreeStack_destroy(arena->header->free_stack);
  // TODO: Modify PAGE_SIZE to make it dynamic
  int err = munmap(arena->header, PAGE_SIZE);
  size_t arena_index = find_bucket_index(arena->header->bucket_size);
  metadata.arenas_created[arena_index] = false;
}

static char* find_free_space(Arena* arena) {
  ArenaHeader* header = arena->header;
  FreeStack* free_stack = header->free_stack;

  if (free_stack->len > 0) {
    PtrResult index = FreeStack_pop(free_stack);
    char* ptr = (char*)index.the.val;

    log_debug("[gmalloc] using free_stack %p \n", ptr);
    return ptr;
  }

  if (header->len == header->capacity) {
    // TODO create a new arena
    perror("Not Implemented yet \n");
    return NULL;
  }

  char* tail_addr = &arena->tail[header->len * header->bucket_size];

  log_debug("[gmalloc] using tail at %p \n", tail_addr);

  return tail_addr;
}

void* gmalloc(size_t size) {
  log_debug("[gmalloc] for size %zu\n", size);

  if (size == 0) {
    return NULL;
  }

  size_t bucket_index = find_bucket_index(size);

  if (bucket_index == (size_t)-1) {
    log_error("Size %zu not handled yet \n", size);
    return NULL;
  }

  if (!metadata.arenas_created[bucket_index]) {
    metadata.arenas[bucket_index] =
        create_arena(bucket_size_from_index(bucket_index));
    // TODO: Use NULL instead of extra space
    metadata.arenas_created[bucket_index] = true;
  }

  Arena* arena = metadata.arenas[bucket_index];

  char* free_space = find_free_space(arena);

  if (free_space == NULL) {
    log_error("Arena Ran out of free space");
    return NULL;
  } else {
    assert((uintptr_t)free_space % 8 == 0);

    arena->header->len++;

    log_debug("[gmalloc] returing ptr %p\n", free_space);
    return (void*)free_space;
  }
}

int gfree(void* ptr) {
  log_debug("[gfree] ptr %p\n", ptr);

  size_t arenas_length = sizeof(metadata.arenas) / sizeof(metadata.arenas[0]);

  Arena* arena;
  bool found = false;

  for (size_t i = 0; i < arenas_length; i++) {
    arena = metadata.arenas[i];

    if (arena == NULL)
      continue;

    // TODO: move to arena.c
    char* tail_addr =
        &arena->tail[arena->header->bucket_size * arena->header->capacity];

    if ((char*)ptr >= (char*)arena && ((char*)ptr <= tail_addr)) {
      found = true;
      break;
    }
  }

  if (!found) {
    return -1;
  }

  ArenaHeader* header = arena->header;

  if (header->len == 0) {
    return -1;
  }

  header->len--;

  if (header->len == 0) {
    // For now since it's only 1 arena per bucket
    // we leave just use the free_stack
    FreeStack_push(header->free_stack, (Byte*)ptr);
    /* destroy_arena(arena); */
  } else {
    FreeStack_push(header->free_stack, (Byte*)ptr);
  }

  return 1;
}

GMAllocMetadata* gmalloc_metadata() { return &metadata; }
