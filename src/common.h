#pragma once

#include "libs/gabe_std.h"
#include "mem.h"
#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <threads.h>
#include <unistd.h>

// Assumed 4kb pages for simplicity
#define PAGE_SIZE 4096
