#include "gmalloc.h"

// TODO: Add more metadata for avoiding lock contention
GMAllocMetadata metadata;

bool has_been_initialized = false;

static void init_metadata(void* arena_init, size_t arena_size) {
  pthread_mutex_init(&metadata.lock, NULL);
  pthread_mutex_lock(&metadata.lock);

  metadata.arena_init = arena_init;
  metadata.arena_size = arena_size;

  /* Arena arena = { */
    /* .init_8 = arena_init, */
    /* .init_16 = (uint8_t*)arena_init + 20000 */
  /* }; */

  /* ArenaMetadata arena_metadata = { */
  /*   .meta_8 = {.last_index = 0, .used_slots = {0} } */
  /* }; */

  /* metadata.arena = arena; */

  has_been_initialized = true;

  pthread_mutex_unlock(&metadata.lock);
}

void gmalloc_init(int pages) {
  int fd = open("/dev/zero", O_RDWR);

  int page_size = 4096;

  // Init arena
  void* arena_init = mmap(NULL, page_size*pages, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

  if (arena_init == MAP_FAILED) {
    perror("Error on mmap");
    exit(1);
  }
  close(fd);

  init_metadata(arena_init, page_size*pages);
}

static void* find_free_space(uint32_t size) {
  perror("Not Implemented yet");
  return NULL;
}

// TODO
// - Check if there is enough space
// - Make it thread safe
// - Return multiple of 8 bytes addresses for alignment
void* gmalloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    printf("Arena size is %zd\n", sizeof(Arena));
    printf("GmallocMetadata size is %zd \n", sizeof(GMAllocMetadata));

    /* if (has_been_initialized == false) { */
    /*   gmalloc_init(1000); */
    /*   printf("Arena init is in %p \n", metadata.arena_init); */
    /* } */

    /* FreeNode* free_node = find_free_node(size); */

    /* if (free_node == NULL) { */

    /* } */

    /* AllocatedHeader* new_header = (AllocatedHeader*)free_node->free_mem_region; */

    /* AllocatedHeader header = {size, MAGIC}; */
    /* *new_header = header; */

    /* // DEBUG */
    /* size_t offset = sizeof(header) + size; */
    /* printf("Size of header is %zu plus mem is %zu \n", sizeof(header), offset); */

    /* defree(free_node, size); */

    /* /\* metadata.free_node_head = (GeneralFreeHeader*)((char*)old_head + offset); *\/ */

    /* printf("New free head is in %p \n", metadata.free_node_head); */

    /* return (new_header+1); */
    return NULL;
}

/* int gfree(void *ptr) { */
/*     return -1; */
/* } */

void gdump() {
    fprintf(stdout, "Arena init is %p\n", metadata.arena_init);
}
