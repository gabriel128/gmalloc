#include "free_stack.h"

FreeStack* FreeStack_new(size_t pages) {
  char* segment = (char*)mmap(NULL, PAGE_SIZE * pages, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

  FreeStack* free_stack = (FreeStack*)segment;
  // List points to the end of the Stack struct to ensure alignment
  free_stack->list = (Byte**)(free_stack + 1);
  free_stack->pages = pages;

  if (segment == MAP_FAILED) {
    perror("Error on creating segment for FreeStack \n");
    exit(1);
  }

  size_t end_of_stack = (size_t)((Byte*)free_stack + PAGE_SIZE * pages);

  log_debug(
      "free_stack starts at %p, list starts at %p, and finishes at %zx \n",
      free_stack, free_stack->list, end_of_stack);

  size_t capacity_in_bytes = (Byte*)end_of_stack - (Byte*)free_stack->list;
  size_t capacity = capacity_in_bytes / sizeof(Byte*);

  free_stack->cursor = 0;
  free_stack->capacity = capacity;
  free_stack->len = 0;

  return free_stack;
}

bool FreeStack_destroy(FreeStack* stack) {
  int err = munmap(stack, PAGE_SIZE * stack->pages);

  if (err != 0) {
    printf("FreeStack unmapping failed \n");
    return false;
  } else {
    return true;
  }
}

bool FreeStack_push(FreeStack* stack, Byte* mem_address) {
  if (stack == NULL) {
    return false;
  }

  if ((stack->len + 1) > stack->capacity) {
    return false;
  }

  stack->list[stack->cursor] = mem_address;
  stack->len++;
  stack->cursor++;

  return true;
}

PtrResult FreeStack_pop(FreeStack* stack) {
  if (stack == NULL) {
    return PTR_ERR("Null stack");
  }

  if (stack->len == 0) {
    return PTR_ERR("Empty stack");
  }

  stack->cursor--;
  stack->len--;
  Byte* val = stack->list[stack->cursor];

  return PTR_OK(val);
}
