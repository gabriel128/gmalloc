#ifndef GMALLOC_H
#define GMALLOC_H

#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

// Assumed 4kb pages for simplicity
#define PAGE_SIZE 4096

void gmalloc_init(int pages);
void *gmalloc(size_t size);
int gfree(void *ptr);
void gdump();

/* typedef struct FreeStackHeader { */
/* } FreeStackHeader; */

typedef struct FreeStack {
  // Points to the next available space
  size_t cursor;
  size_t capacity;
  size_t len;
  // Handles up to Arenas of ~65000 buckets (i.e. 16bit long index)
  uint16_t *list;
} FreeStack;

static FreeStack* FreeStack_new() {
  // We get a heap memory segment of 16 pages
  char* segment = (char*) mmap(
      NULL, PAGE_SIZE * 16, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

  FreeStack* free_stack = (FreeStack*)segment;
  // List points to the end of the Stack struct to ensure alignment
  free_stack->list = (uint16_t*)(free_stack+1);

  if (segment == MAP_FAILED) {
    perror("Error on creating segment for FreeStack \n");
    exit(1);
  }

  size_t end_of_stack = (size_t)((char *)free_stack + PAGE_SIZE * 16);
  printf("free_stack starts at %p, list starts at %p, and finishes at %zx \n",
         free_stack,
         free_stack->list,
         end_of_stack);

  size_t capacity_in_bytes = (char*)end_of_stack - (char*)free_stack->list;
  size_t capacity = capacity_in_bytes / sizeof(uint16_t);

  free_stack->cursor = 0;
  free_stack->capacity = capacity;
  free_stack->len = 0;

  return free_stack;
}

static bool FreeStack_destroy(FreeStack *stack) {
  int err = munmap(stack, 10 * sizeof(int));

  if (err != 0) {
    printf("FreeStack unmapping failed \n");
    return false;
  } else {
    return true;
  }
}

static bool FreeStack_push(FreeStack *stack, uint16_t slot_index) {
  if (stack == NULL) {
    return false;
  }

  if ((stack->len + 1) > stack->capacity) {
    return false;
  }

  stack->list[stack->cursor] = slot_index;
  stack->len++;
  stack->cursor++;

  return true;
}

typedef enum {
    RESULT_OK,
    RESULT_ERR
} ResultKind;

typedef struct Result {
   ResultKind kind;
    union {
        uint64_t val;
        char* err;
    } the;
} Result;

#define OK(value)  ((Result){RESULT_OK, {.val=value}})
#define ERR(error) ((Result){RESULT_ERR, {.err=error}})
#define IS_OK(result) ((result).kind == RESULT_OK)
#define IS_ERR(result) ((result).kind == RESULT_ERR)

static Result FreeStack_pop(FreeStack *stack) {
  if (stack == NULL) {
      return ERR("Null stack");
  }

  if (stack->len == 0) {
      return ERR("Empty stack");
  }

  stack->cursor--;
  stack->len--;
  uint16_t val = stack->list[stack->cursor];

  return OK(val);

  /* if ((stack->len + 1) > stack->capacity) { */
  /*   return false; */
  /* } */

  /* stack->list[stack->cursor] = slot_index; */
  /* stack->len++; */
  /* stack->cursor++; */

  /* return true; */
}

typedef struct ArenaHeader {
  uint16_t bucket_size;
  size_t size_bytes;
  size_t capacity;
  size_t len;
  // Points to the next available slot
  size_t cursor;
} ArenaHeader;

typedef struct Arena {
  ArenaHeader header;
  FreeStack *free_stack;
  void *arena_start_ptr;
} Arena;

typedef struct GMallocMetadata {
  pthread_mutex_t lock;
  size_t arena_size;
  void *arena_init;
  Arena arena;
} GMAllocMetadata;

#endif /* GMALLOC_H */
