#pragma once

#include "common.h"

void* mem_zero_init(int pages);
void* mem_init(int pages);
size_t calculate_capacity(byte*, uint32_t, byte*, size_t);
