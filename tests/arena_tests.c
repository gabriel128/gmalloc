#include <criterion/criterion.h>

#include <stddef.h>

#include "../src/arena.h"

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

Test(arena_tests, push_mem_blocks_between_capacity_8) {
    Arena* arena = Arena_create(8, 1);
    ArenaHeader* header = &arena->header;

    MemBlock* block = Arena_push_mem_block(arena);
    *(long*)block->data = 42;

    cr_assert_eq(header->len, 1, "Len is %zu \n", header->len);

    MemBlock* block2 = Arena_push_mem_block(arena);
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

    for (int i = 0; i < header->capacity; i++) {
      MemBlock* block = Arena_push_mem_block(arena);
      cr_assert_not_null(block);

      *(int*)block->data = i;

      cr_assert_eq((int)header->len, i+1, "Len is %zu \n", header->len);
      cr_assert_eq(block->arena, arena, "Block arena is %p, arena is %p\n", block->arena, arena);
      cr_assert_eq(*(int*)block->data, i, "Block data is %d\n", *(int*)block->data);
    }

    // It doesn't create a new Arena
    cr_assert_null(arena->next_arena);

    // Creates new Arena
    MemBlock* block = Arena_push_mem_block(arena);
    cr_assert_not_null(arena->next_arena);
}
