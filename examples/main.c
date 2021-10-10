#include "../src/gmalloc.h"
#include <sys/resource.h>

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
    /* int init_mem; */
    /* get_mem(&init_mem); */
    long init_mem = get_mem_usage();

    for (int i = 0; i < 10000000; i++) {
      int* a = gmalloc(8);
      /* *a = 4; */
      memset(a, 1, 8);
      /* printf("a is %d i is %d \n", *a, i); */
    }
    /* memset(x, 1, 8); */
    /* *x = 0xFFFFFFFF; */
    /* gfree(x); */

    /* int after_free_mem; */
    /* get_mem(&after_free_mem); */

    long after_free_mem = get_mem_usage();
    printf("Mem usage: %ld MB\n", (after_free_mem - init_mem) / 1024^2);
/* ./bin/example  0.26s user 0.54s system 60% cpu 1.333 total */


    /* while(1); */

    return 0;
}
