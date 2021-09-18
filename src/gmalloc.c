#include "gmalloc.h"


/*
  8 bytes
  free bucket -> |  0  |   |   |   |   |   |
  16 bytes
  |  0  |   |   |   |   |   |
  32 bytes
  |  0  |   |   |   |   |   |
  64 bytes
  |  0  |   |   |   |   |   |
  128 bytes
  |  0  |   |   |   |   |   |
  256 bytes
  |  0  |   |   |   |   |   |
  512 bytes
  |  0  |   |   |   |   |   |
*/

// TODO: Add more metadata for avoiding lock contention
AllocMetadata metadata;
bool has_been_initialized = false;

static void init_metadata(void* arena_int, void* free_page, size_t arena_size) {
  pthread_mutex_init(&metadata.lock, NULL);
  pthread_mutex_lock(&metadata.lock);

  metadata.arena_init = arena_int;
  metadata.alloc_head = arena_int;
  metadata.free_node_head = free_page;
  metadata.arena_size = arena_size;

  has_been_initialized = true;

  // Init Free headers linked list
  FreeNode free_node_header = {UINT_MAX, NULL, NULL, metadata.arena_init};
  *metadata.free_node_head = free_node_header;

  pthread_mutex_unlock(&metadata.lock);
}

void gmalloc_init(int pages) {
  int fd = open("/dev/zero", O_RDWR);

  int page_size = getpagesize();

  // Init arena
  void* arena_init = mmap(NULL, page_size*pages, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  // Creates 1 page for Free data structure
  void* free_page = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

  if (arena_init == MAP_FAILED || free_page == MAP_FAILED) {
    perror("Error on mmap");
    exit(1);
  }
  close(fd);

  init_metadata(arena_init, free_page, page_size*pages);
}

/* void* create_free_page() { */
/*   int fd = open("/dev/zero", O_RDWR); */
/*   int page_size = getpagesize(); */

/*   void *ptr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0); */

/*   if (ptr == MAP_FAILED) { */
/*     perror("Error on mmap"); */
/*     exit(1); */
/*   } */
/*   close(fd); */
/*   return ptr; */
/* } */


static FreeNode* find_free_node(uint32_t size) {
  FreeNode* node = metadata.free_node_head;

  while(true)  {
    if (size < node->size) {
      break;
    } else if () {

    }

  }

  perror("Not Implemented yet");
  return node;
}

// Make a Free Node not free anyumore
// The head of the 
static void defree(FreeNode* free_node, uint32_t size) {
  if (free_node_is_head(free_node, metadata)) {
    free_node->size = size;
    free_node->free_mem_region = free_node->free_mem_region + sizeof(AllocatedHeader) + size;
  } else {
    perror("Not Implemented yet");
  }
}

// TODO
// - Check if there is enough space
// - Make it thread safe
// - Return multiple of 8 bytes addresses for alignment
void* gmalloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    if (has_been_initialized == false) {
      gmalloc_init(1000);
      printf("Arena init is in %p \n", metadata.arena_init);
    }

    FreeNode* free_node = find_free_node(size);

    if (free_node == NULL) {

    }

    AllocatedHeader* new_header = (AllocatedHeader*)free_node->free_mem_region;

    AllocatedHeader header = {size, MAGIC};
    *new_header = header;

    // DEBUG
    size_t offset = sizeof(header) + size;
    printf("Size of header is %zu plus mem is %zu \n", sizeof(header), offset);

    defree(free_node, size);

    /* metadata.free_node_head = (GeneralFreeHeader*)((char*)old_head + offset); */

    printf("New free head is in %p \n", metadata.free_node_head);

    return (new_header+1);
}

/* int gfree(void *ptr) { */
/*     return -1; */
/* } */

void gdump() {
    fprintf(stdout, "Arena init is %p\n", metadata.arena_init);
}
