#include "../src/gmalloc.h"
#include <sys/resource.h>
#include <pthread.h>
#include <time.h>
#include <sys/wait.h>


/* #define ALLOC_SIZE 1<<26 */
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

void reusing_freed_bench() {
    printf("============================= \n");
    printf("Reusing Freed memory Bench \n");

    printf("============================= \n");
    printf("Reusing Freed memory Bench \n");

    long long iter = 1000000;
    static int* allocs[500];

    long init_mem = get_mem_usage();
    clock_t start_time = clock();

    for (int j = 0; j < iter; j++) {
        for(int i = 0; i < 500; i++) {
          allocs[i] = malloc(8);
          *allocs[i] = 42;
        }

        for(int i = 0; i < 500; i++) {
          free(allocs[i]);
        }
    }

    long after_free_mem = get_mem_usage();
    double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    printf("============================= \n");
    printf("Result => Time: %f,  Mem usage: %lf MB \n", elapsed_time, (double)(after_free_mem - init_mem) / 1024);
    printf("============================= \n");
}

void single_thread_mixed_freeing_bench() {
    printf("============================= \n");
    printf("2. Running Single Thread With Mixed Freeing on the way, Running %d Iterations \n", ALLOC_SIZE);

    static int* allocs[ALLOC_SIZE];

    long init_mem = get_mem_usage();
    clock_t start_time = clock();

    for (int i = 0; i < ALLOC_SIZE; i++) {
      allocs[i] = malloc(8);
      *allocs[i] = 42;
      if (i > 500) {
          free(allocs[i-500]);
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
      allocs[i] = malloc(8);
    }

    for (int i = 0; i < ALLOC_SIZE; i++) {
      memset(allocs[i], 1, 8);
    }

    for (int i = 0; i < ALLOC_SIZE; i++) {
      free(allocs[i]);
    }

    long after_free_mem = get_mem_usage();
    double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;

    printf("============================= \n");
    printf("1. Result => Time: %f,  Mem usage: %lf MB \n", elapsed_time, (double)(after_free_mem - init_mem) / 1024);
    printf("============================= \n");
}

long* allocs_multi_tr[ALLOC_SIZE];
const int threads_qty = 10;

void* alloc_it(void *args)
{
    int* init = (int*)args;
    int total_numbers = ALLOC_SIZE / threads_qty;

    /* printf("Test %d %d %d \n", *init, (*init * total_numbers), ((*init * total_numbers) + total_numbers)); */
    for(int i = *init; i < (*init + total_numbers); i++) {
      long* a = malloc(8);
      allocs_multi_tr[i] = a;
    }
    return NULL;
}

void* set_it(void *args)
{
    int* init = (int*)args;
    int total_numbers = ALLOC_SIZE / threads_qty;

    for(int i = *init; i < (*init + total_numbers); i++) {
      *allocs_multi_tr[i] = 42;
    }
    return NULL;
}


/* void* free_it(void *args) */
/* { */
/*     int* init = (int*)args; */
/*     int total_numbers = ALLOC_SIZE / threads_qty; */
/*     for(int i = *init; i < (*init + total_numbers); i++) { */
/*       long* */
/*       free(allocs_multi_tr[i]); */
/*     } */
/*     return NULL; */
/* } */

void multi_thread_bench() {
    printf("============================= \n");
    printf("3. Running Multi Thread Bench, Running %d Iterations \n", ALLOC_SIZE);
    printf("============================= \n");

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    long init_mem = get_mem_usage();
    clock_t start_time = clock();

    pthread_t alloc_thread_ids[threads_qty];

    int a = 0;
    for(int i = 0; i < threads_qty; i++) {
        int* ptr = gmalloc(sizeof(int));
        *ptr = i;
        pthread_create(&alloc_thread_ids[i], NULL, alloc_it, ptr);
    }

    for(int i = 0; i < threads_qty; i++) {
        pthread_join(alloc_thread_ids[i], NULL);
    }

    for(int i = 0; i < threads_qty; i++) {
        int* ptr = gmalloc(sizeof(int));
        *ptr = i;
        pthread_create(&alloc_thread_ids[i], NULL, set_it, ptr);
    }

    for(int i = 0; i < threads_qty; i++) {
        pthread_join(alloc_thread_ids[i], NULL);
    }

    long after_free_mem = get_mem_usage();
    double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;

    /* for(int i = 0; i < ALLOC_SIZE; i++) */
    /*     assert(*allocs_multi_tr[0] == 42); */

    printf("============================= \n");
    printf("3. Result => Time: %f,  Mem usage: %lf MB \n", elapsed_time, (double)(after_free_mem - init_mem) / 1024);
    printf("============================= \n");
}

int main() {
    /* single_thread_allatonce_bench(); */
    /* multi_thread_bench(); */
    /* single_thread_mixed_freeing_bench(); */
    reusing_freed_bench();

    return 0;
}
