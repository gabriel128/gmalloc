#include "../src/gmalloc.h"
#include <sys/resource.h>
#include <math.h>

#define ALLOC_SIZE 1<<25

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

int* allocs[ALLOC_SIZE];

int main() {
    printf("%d Iterations \n", ALLOC_SIZE);
    long init_mem = get_mem_usage();

    for (int i = 0; i < ALLOC_SIZE; i++) {
      allocs[i] = malloc(8);
    }

    for (int i = 0; i < ALLOC_SIZE; i++) {
      memset(allocs[i], 1, 8);
    }

    for (int i = 0; i < ALLOC_SIZE; i++) {
      free(allocs[i]);
    }

    long after_free_mem = get_mem_usage();
    printf("Mem usage: %lf MB \n", (double)(after_free_mem - init_mem) / 1024);

    /* while(true); */
    return 0;
}
