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
      // TODO assing FreeStack pages accordingly
      .free_stack = FreeStack_new(1),
      .arena_start_ptr = mem_ptr,
      .tail = mem_ptr
  };

  return arena;
}

// TODO
static bool destroy_arena(Arena arena) {
  return false;
}

// TODO: Make this a Result/Option?
static char* find_free_space(Arena* arena) {
  ArenaHeader* header = &arena->header;
  FreeStack* free_stack = arena->free_stack;

  if (free_stack->len > 0) {
    PtrResult index = FreeStack_pop(free_stack);
    char* ptr = (char*)index.the.val;

    log_debug("[gmalloc] using free_stack %p \n", ptr);
    return ptr;
  }

  if (header->len >= header->capacity) {
    // TODO create a new arena
    return NULL;
  }

  char* old_tail = arena->tail;
  char* next_tail = arena->tail + header->bucket_size;
  arena->tail = next_tail;
  header->len++;

  log_debug("[gmalloc] using tail, old_tail was %p, new tail is %p \n", old_tail, next_tail);

  return old_tail;
}

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

size_t bucket_size_from_index(size_t index) {
  return 1<<(index+3);
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
    metadata.arenas[bucket_index] = create_arena(bucket_size_from_index(bucket_index));
    metadata.arenas_created[bucket_index] = true;
  }

  Arena* arena = &metadata.arenas[bucket_index];

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

  FreeStack_push(arena.free_stack, (Byte*)ptr);

  // TODO
  /* if(header.len == header.capacity && FreeStack_empty(arena.free_stack)) { */
  /*   destroy arena */
  /* } */

  return 1;
}

GMAllocMetadata* gmalloc_metadata() {
  return &metadata;
}
