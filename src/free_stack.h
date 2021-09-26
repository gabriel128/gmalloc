#pragma once

#include "common.h"

// Holds a list of pointers, pointing to where the free memory is.
// The reason for using indexes instead of a pointer list
// is to save up some memory.
// The arena using this stack is in charge of doing the
// TODO extract non-list to header to header
typedef struct FreeStack {
  size_t pages;
  // Points to the next available space
  size_t cursor;
  size_t capacity;
  size_t len;
  // Hols up to ~512 per page (i.e. Page - header)
  Byte** list;
} FreeStack;

FreeStack* FreeStack_new(size_t size);
bool FreeStack_destroy(FreeStack* stack);
bool FreeStack_push(FreeStack* stack, Byte* slot_index);
PtrResult FreeStack_pop(FreeStack* stack);
PtrResult FreeStack_top(FreeStack* stack);

// TODO: implement
FreeStack* FreeStack_new_with_capacity(size_t capacity);
size_t FreeStack_len(FreeStack* stack);
bool FreeStack_is_empty(FreeStack* stack);
