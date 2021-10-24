#include "free_stack.h"

FreeStack* FreeStack_new(size_t pages) {
  byte* segment = (byte*)mmap(NULL, PAGE_SIZE * pages, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

  log_debug("Maping FreeStack %zu", pages);
  FreeStack* free_stack = (FreeStack*)segment;
  free_stack->pages = pages;

  if (segment == MAP_FAILED) {
    perror("Error on creating segment for FreeStack \n");
    exit(1);
  }

  size_t end_of_stack = (size_t)(segment + PAGE_SIZE * pages);

  size_t capacity_in_bytes = (byte*)end_of_stack - (byte*)free_stack->list;
  size_t capacity = capacity_in_bytes / sizeof(byte*);

  free_stack->cursor = 0;
  free_stack->capacity = capacity;
  free_stack->len = 0;

  return free_stack;
}

bool FreeStack_is_empty(FreeStack* stack) { return stack->len == 0; }

bool FreeStack_destroy(FreeStack* stack) {
  log_debug("Unmaping FreeStack %zu \b", stack->pages);
  int err = munmap(stack, PAGE_SIZE * stack->pages);

  if (err != 0) {
    printf("FreeStack unmapping failed \n");
    return false;
  } else {
    return true;
  }
}

bool FreeStack_push(FreeStack* stack, byte* mem_address) {
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
  byte* val = stack->list[stack->cursor];

  return PTR_OK(val);
}
