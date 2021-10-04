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
