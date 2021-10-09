#include <criterion/criterion.h>

#include <stddef.h>

#include "../src/arena.h"
#include <signal.h>

Test(arena_tests, eight_byte_bucket_creation) {
    printf("ArenaHeader size %zu \n", sizeof(ArenaHeader));
    printf("Arena size %zu \n", sizeof(Arena));
    printf("Offset of blocks %zu \n", offsetof(Arena, next_arena));

    Arena* arena = Arena_create(8, 1);

    cr_assert_not_null(arena->free_stack);
    cr_assert_not_null(arena);
    cr_assert_null(arena->next_arena);
    cr_assert_null(arena->prev_arena);

    ArenaHeader header = arena->header;

    cr_assert_eq(header.bucket_size, 8);
    cr_assert_eq(header.mem_pages, 1);
    cr_assert_eq(header.len, 0);
    cr_assert_eq(header.capacity, 253, "Capacity is %zu \n", header.capacity);

}

Test(arena_tests, sixteen_byte_bucket_creation) {
    Arena* arena = Arena_create(16, 1);

    cr_assert_not_null(arena->free_stack);
    cr_assert_not_null(arena);
    cr_assert_null(arena->next_arena);
    cr_assert_null(arena->prev_arena);

    ArenaHeader header = arena->header;

    cr_assert_eq(header.bucket_size, 16);
    cr_assert_eq(header.mem_pages, 1);
    cr_assert_eq(header.len, 0);
    cr_assert_eq(header.capacity, 168, "Capacity is %zu \n", header.capacity);
}

Test(arena_tests, get_mem_blocks_between_capacity_8) {
    Arena* arena = Arena_create(8, 1);
    ArenaHeader* header = &arena->header;

    MemBlock* block = Arena_get_mem_block(arena);
    *(long*)block->data = 42;

    cr_assert_eq(header->len, 1, "Len is %zu \n", header->len);

    MemBlock* block2 = Arena_get_mem_block(arena);
    *(long*)block2->data = 43;

    cr_assert_eq(header->len, 2, "Len is %zu \n", header->len);

    cr_assert_eq(block->arena, arena, "Block arena is %p, arena is %p\n", block->arena, arena);
    cr_assert_eq(*(long*)block->data, 42, "Block data is %ld\n", *(long*)block->data);

    cr_assert_eq(block2->arena, arena, "Block arena is %p, arena is %p\n", block2->arena, arena);
    cr_assert_eq(*(long*)block2->data, 43, "Block data is %ld\n", *(long*)block2->data);
}

Test(arena_tests, can_allocate_capacity8) {
    Arena* arena = Arena_create(8, 1);
    ArenaHeader* header = &arena->header;

    for (int i = 0; i < (int)header->capacity; i++) {
      MemBlock* block = Arena_get_mem_block(arena);
      cr_assert_not_null(block);

      *(int*)block->data = i;

      cr_assert_eq((int)header->len, i+1, "Len is %zu \n", header->len);
      cr_assert_eq(block->arena, arena, "Block arena is %p, arena is %p\n", block->arena, arena);
      cr_assert_eq(*(int*)block->data, i, "Block data is %d\n", *(int*)block->data);
    }

    // It doesn't create a new Arena
    cr_assert_null(arena->next_arena);

    // Creates new Arena
    MemBlock* block = Arena_get_mem_block(arena);
    cr_assert_not_null(arena->next_arena);
}

Test(arena_tests, freeing_on_head_arena_does_not_destroy_arena) {
    Arena* arena = Arena_create(8, 1);
    MemBlock* block = Arena_get_mem_block(arena);
    bool succeeded = Arena_free_mem_block(block);

    cr_assert(succeeded);
    cr_assert_eq(arena->free_stack->len, 1);
    cr_assert_null(arena->next_arena);
}

Test(arena_tests, freeing_on_nonhead_arena_destroys_arena, .signal = SIGSEGV) {
    Arena* arena = Arena_create(PAGE_SIZE/2, 1);
    cr_assert_not_null(arena);

    MemBlock* block1 = Arena_get_mem_block(arena);
    MemBlock* block2 = Arena_get_mem_block(arena);

    cr_assert_neq(block1->arena, block2->arena);
    cr_assert_not_null(arena->next_arena);
    cr_assert_eq(arena, block2->arena->prev_arena);
    cr_assert_eq(arena->next_arena, block2->arena);

    bool succeeded = Arena_free_mem_block(block2);
    cr_assert(succeeded);
    cr_assert_null(arena->next_arena);

    // Should segfault since if was unmapped
    *(int*)block2->data = 4;
}

Test(arena_tests, freeing_on_non_head_non_tail_arena, .signal = SIGSEGV) {
    Arena* arenas_head = Arena_create(PAGE_SIZE/2, 1);
    cr_assert_not_null(arenas_head);

    MemBlock* block1 = Arena_get_mem_block(arenas_head);
    // This will go to the tail
    MemBlock* block2 = Arena_get_mem_block(arenas_head);
    // This will go in the middle
    MemBlock* block3 = Arena_get_mem_block(arenas_head);

    cr_assert_neq(block1->arena, block2->arena);
    cr_assert_neq(block1->arena, block3->arena);
    cr_assert_neq(block2->arena, block3->arena);

    cr_assert_eq(arenas_head->prev_arena, NULL);
    cr_assert_eq(arenas_head->next_arena, block3->arena);

    cr_assert_eq(block3->arena->prev_arena, block1->arena);
    cr_assert_eq(block3->arena->next_arena, block2->arena);

    cr_assert_eq(block2->arena->prev_arena, block3->arena);
    cr_assert_eq(block2->arena->next_arena, NULL);

    // Destroying the middle Arena
    bool succeeded = Arena_free_mem_block(block3);
    cr_assert(succeeded);

    cr_assert_eq(arenas_head->prev_arena, NULL);
    cr_assert_eq(arenas_head->next_arena, block2->arena);

    cr_assert_eq(block2->arena->prev_arena, block1->arena);
    cr_assert_eq(block2->arena->next_arena, NULL);

    // Segfault when setting data on unmapped memory
    *(int*)block3->data = 4;
}
