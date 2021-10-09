#include "mem.h"

void* mem_zero_init(int pages) {
  int fd = open("/dev/zero", O_RDWR);

  int page_size = PAGE_SIZE;

  void* ptr =
      mmap(NULL, page_size * pages, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

  if (ptr == MAP_FAILED) {
    perror("Error on mmap");
    exit(1);
  }
  close(fd);

  return ptr;
}

size_t calculate_capacity(byte* init, uint32_t pages, byte* block_init, size_t block_size) {
  size_t end_of_arena = (uintptr_t)(init + PAGE_SIZE * pages);
  return (size_t)((end_of_arena - (uintptr_t)block_init) / block_size);
}
