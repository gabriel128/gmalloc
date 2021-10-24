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
    metadata.arenas[bucket_index] = Arenarray_new(bucket_size_from_index(bucket_index));
    metadata.arenas_created[bucket_index] = true;
  }

  Arenarray* arenarray = &metadata.arenas[bucket_index];

  MemBlock* mem_block = Arenarray_find_mem_block(arenarray);

  if (mem_block == NULL) {
    log_error("Couldn't find a suitable mem_block for size");
    return NULL;
  } else {
    assert((uintptr_t)mem_block->data % 8 == 0);
    log_debug("[gmalloc] returing ptr %p\n", mem_block->data);

    return (void*)mem_block->data;
  }
}

int gfree(void* ptr) {
  log_debug("[gfree] ptr %p\n", ptr);

  MemBlock* mem_block = (MemBlock*)((byte*)ptr - sizeof(MemBlock));

  bool freed = Arena_free_mem_block(mem_block);

  if (!freed) {
    log_error("Memory couldn't be freed");
    return -1;
  }

  return 1;
}

GMAllocMetadata* gmalloc_metadata() { return &metadata; }
