#include "../src/gmalloc.h"
#include <sys/resource.h>
#include <pthread.h>
#include <time.h>
#include <sys/wait.h>


/* #define ALLOC_SIZE 1<<25 */
#define ALLOC_SIZE 100000000

long get_mem_usage() {
    struct rusage usage;

    getrusage(RUSAGE_SELF, &usage);

    // in kb
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


void single_thread_mixed_freeing_bench() {
    printf("============================= \n");
    printf("2. Running Single Thread With Mixed Freeing on the way, Running %d Iterations \n", ALLOC_SIZE);

    static int* allocs[ALLOC_SIZE];

    long init_mem = get_mem_usage();
    clock_t start_time = clock();

    for (int i = 0; i < ALLOC_SIZE; i++) {
      allocs[i] = gmalloc(8);
      memset(allocs[i], 1, 8);
      if (i > 10000) {
          gfree(allocs[i-1000]);
      }
    }

    long after_free_mem = get_mem_usage();
    double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    printf("============================= \n");
    printf("2. Result => Time: %f,  Mem usage: %lf MB \n", elapsed_time, (double)(after_free_mem - init_mem) / 1024);
    printf("============================= \n");
}

void single_thread_allatonce_bench() {
    printf("============================= \n");
    printf("1. Running Single Thread Bench Allocating all at once, Running %d Iterations \n", ALLOC_SIZE);
    printf("============================= \n");

    static int* allocs[ALLOC_SIZE];

    long init_mem = get_mem_usage();
    clock_t start_time = clock();

    for (int i = 0; i < ALLOC_SIZE; i++) {
      allocs[i] = gmalloc(8);
    }

    for (int i = 0; i < ALLOC_SIZE; i++) {
      memset(allocs[i], 1, 8);
    }

    for (int i = 0; i < ALLOC_SIZE/2; i++) {
      gfree(allocs[i]);
    }

    long after_free_mem = get_mem_usage();
    double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;

    printf("============================= \n");
    printf("1. Result => Time: %f,  Mem usage: %lf MB \n", elapsed_time, (double)(after_free_mem - init_mem) / 1024);
    printf("============================= \n");
}

int* allocs_multi_tr[ALLOC_SIZE];

void *alloc(void *args)
{
    printf("blah \n");
    return NULL;
}

void multi_thread_bench() {
    printf(" ============================= \n");
    printf("Running Multi Thread Bench, Running %d Iterations \n", ALLOC_SIZE);

    long init_mem = get_mem_usage();

    int threads_qty = 20;
    pthread_t alloc_thread_ids[threads_qty];

    for(int i = 0; i < threads_qty; i++) {
        pthread_create(&alloc_thread_ids[i], NULL, alloc, &i);
    }

    for(int i = 0; i < threads_qty; i++) {
        pthread_join(alloc_thread_ids[i], NULL);
    }

    /* pthread_t free_thread_ids[threads_qty]; */
    /* for(int i = 0; i < threads_qty; i++) { */

    /* } */

    long after_free_mem = get_mem_usage();
    printf("Mem usage: %lf MB \n", (double)(after_free_mem - init_mem) / 1024);
}

int main() {
    int child = fork();

    if(child == 0 ) {
        single_thread_allatonce_bench();
    } else {
        single_thread_mixed_freeing_bench();
    }

    waitpid(child, 0, 0);

    return 0;
}
