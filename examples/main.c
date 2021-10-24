#include "../src/gmalloc.h"
#include <sys/resource.h>

#define ALLOC_SIZE 1

long get_mem_usage() {
    struct rusage usage;

    getrusage(RUSAGE_SELF, &usage);

    return usage.ru_maxrss;
}
void get_mem(int* currVirtMem) {

    // stores each word in status file
    char buffer[1024] = "";

    // linux file contains this-process info
    FILE* file = fopen("/proc/self/status", "r");

    // read the entire file
    while (fscanf(file, " %1023s", buffer) == 1) {

        if (strcmp(buffer, "VmPeak:") == 0) {
            fscanf(file, " %d", currVirtMem);
        }
    }
    fclose(file);
}


int main() {
    static int* allocs[ALLOC_SIZE];

    long init_mem = get_mem_usage();
    clock_t start_time = clock();

    for (int i = 0; i < ALLOC_SIZE; i++) {
      allocs[i] = gmalloc(8);
    }

    for (int i = 0; i < ALLOC_SIZE; i++) {
      *allocs[i] = 42;
    }


    printf("alloc is %d \n", *allocs[0]);
    for (int i = 0; i < ALLOC_SIZE; i++) {
      gfree(allocs[i]);
    }

    return 0;
}
