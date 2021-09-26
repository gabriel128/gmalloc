#include "gmalloc.h"

thread_local GMAllocMetadata metadata;

bool has_been_initialized = false;

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

static Arena create_arena(uint16_t bucket_size) {
  uint32_t pages = 1;
  void* mem_ptr = mem_init(pages);

  ArenaHeader arena_header = {
     .bucket_size = bucket_size,
     .size_in_bytes = PAGE_SIZE  * pages,
     .capacity = PAGE_SIZE * pages / bucket_size,
     .len = 0
  };

  Arena arena = {
      .header = arena_header,
      .free_stack = FreeStack_new(),
      .arena_start_ptr = mem_ptr,
      .tail = mem_ptr
  };

  return arena;
}

static bool destroy_arena(Arena arena) {
  return false;
}

// TODO: Make this a Result/Option?
static char* find_free_space(Arena* arena) {
  ArenaHeader* header = &arena->header;
  FreeStack* free_stack = arena->free_stack;

  if (free_stack->len > 0) {
    UIntResult index = FreeStack_pop(free_stack);
    char* ptr = arena->arena_start_ptr + (index.the.val * header->bucket_size);

    log_debug("[gmalloc] using free_stack %p \n", ptr);
    return ptr;
  }

  if (header->len >= header->capacity) {
    return NULL;
  }

  char* old_tail = arena->tail;
  char* next_tail = arena->tail + header->bucket_size;
  arena->tail = next_tail;

  log_debug("[gmalloc] using tail, old_tail was %p, new tail is %p \n", old_tail, next_tail);

  return old_tail;
}

void* gmalloc(size_t size) {
  log_debug("[gmalloc] for size %zu\n", size);

  if (size == 0) {
    return NULL;
  }
  /* log_debug("Arena struct size is %zd\n", sizeof(Arena)); */
  /* log_debug("GmallocMetadata struct size is %zd \n", sizeof(GMAllocMetadata)); */

  if (size > 8) {
    log_error("Size %zu not handled yet \n", size);
    return NULL;
  }

  if (!metadata.arenas_created[0]) {
    metadata.arenas[0] = create_arena(8);
    metadata.arenas_created[0] = true;
  }

  Arena* arena = &metadata.arenas[0];

  char* free_space = find_free_space(arena);

  if (free_space == NULL) {
    log_error("Arena Ran out of free space");
    return NULL;
  } else {
    assert((uintptr_t)free_space % 8 == 0);
    return (void*)free_space;
  }
}

int gfree(void* ptr) {
  log_debug("[gfree] ptr %p\n", ptr);

  size_t arenas_length = sizeof(metadata.arenas) / sizeof(metadata.arenas[0]);

  Arena arena;
  bool found = false;

  for (size_t i = 0; i < arenas_length; i++) {
    arena = metadata.arenas[i];

    if ((char*)ptr >= arena.arena_start_ptr && (char*)ptr < arena.tail)   {
       found = true;
       break;
    }
  }

  if(!found)
    return -1;

  ArenaHeader header = arena.header;

  size_t index = ((char*) ptr - arena.arena_start_ptr) / header.bucket_size ;
  FreeStack_push(arena.free_stack, index);

  return 1;
}

GMAllocMetadata get_metadata() {
  return metadata;
}
