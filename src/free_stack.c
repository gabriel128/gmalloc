#include "free_stack.h"

FreeStack* FreeStack_new() {
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
  log_debug("free_stack starts at %p, list starts at %p, and finishes at %zx \n",
         free_stack,
         free_stack->list,
         end_of_stack);
  log_info("free_stack starts at %p, list starts at %p, and finishes at %zx \n",
         free_stack,
         free_stack->list,
         end_of_stack);
  log_error("free_stack starts at %p, list starts at %p, and finishes at %zx \n",
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

bool FreeStack_destroy(FreeStack *stack) {
  int err = munmap(stack, 10 * sizeof(int));

  if (err != 0) {
    printf("FreeStack unmapping failed \n");
    return false;
  } else {
    return true;
  }
}

bool FreeStack_push(FreeStack *stack, uint16_t slot_index) {
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

Result FreeStack_pop(FreeStack *stack) {
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
}
