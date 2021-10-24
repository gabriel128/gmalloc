#pragma once

#include "arenarray.h"
#include "common.h"
// Assumed 4kb pages for simplicity
#define PAGE_SIZE 4096
#define ARENAS_QTY 7

typedef struct GMallocMetadata {
  Arenarray arenas[ARENAS_QTY];
  bool arenas_created[ARENAS_QTY];
} GMAllocMetadata;

void* gmalloc(size_t size);
int gfree(void* ptr);
GMAllocMetadata* gmalloc_metadata();
