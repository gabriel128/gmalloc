#pragma once

#include "common.h"
#include "arena.h"
// Assumed 4kb pages for simplicity
#define PAGE_SIZE 4096

typedef struct GMallocMetadata {
  Arena* arenas[7];
} GMAllocMetadata;

void gmalloc_init(int pages);
void* gmalloc(size_t size);
int gfree(void* ptr);
void gdump();
GMAllocMetadata* gmalloc_metadata();
