#pragma once

#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "libs/gabe_std.h"
#include <string.h>

// Assumed 4kb pages for simplicity
#define PAGE_SIZE 4096
