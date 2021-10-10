#include "mem.h"

void* mem_zero_init(int pages) {
  int fd = open("/dev/zero", O_RDWR);

  int page_size = PAGE_SIZE;

  void* ptr =
      mmap(NULL, page_size * pages, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

  /* log_debug("[Mem_zero_init] Page_size created %d, %d, %p \n", page_size, pages, ptr); */
  printf("[Mem_zero_init] Page_size created %d, %d, %p \n", page_size, pages, ptr);

  if (ptr == MAP_FAILED) {
    perror("Error on mmap");
    exit(1);
  }
  close(fd);

  return ptr;

  /* void* segment = (byte*)mmap(NULL, PAGE_SIZE * pages, PROT_READ | PROT_WRITE, */
  /*                             MAP_PRIVATE | MAP_ANONYMOUS, 0, 0); */

  /* if (segment == MAP_FAILED) { */
  /*   perror("Error on creating segment for FreeStack \n"); */
  /*   exit(1); */
  /* } */

  /* return segment; */
}

void* mem_init(int pages) {
  void* segment = (byte*)mmap(NULL, PAGE_SIZE * pages, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

  if (segment == MAP_FAILED) {
    perror("Error on creating segment for FreeStack \n");
    exit(1);
  }

  return segment;
}

size_t calculate_capacity(byte* init, uint32_t pages, byte* block_init, size_t block_size) {
  size_t end_of_arena = (uintptr_t)(init + PAGE_SIZE * pages);
  return (size_t)((end_of_arena - (uintptr_t)block_init) / block_size);
}
