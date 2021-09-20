#pragma once

#include "common.h"

typedef struct FreeStack {
  // Points to the next available space
  size_t cursor;
  size_t capacity;
  size_t len;
  // Handles up to Arenas of ~65000 buckets (i.e. 16bit long index)
  uint16_t* list;
} FreeStack;

FreeStack* FreeStack_new();
bool FreeStack_destroy(FreeStack* stack);
bool FreeStack_push(FreeStack* stack, uint16_t slot_index);
Result FreeStack_pop(FreeStack* stack);
