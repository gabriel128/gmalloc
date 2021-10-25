#include <criterion/criterion.h>

#include <stddef.h>

#include "../src/arena.h"
#include <signal.h>

Test(arena_tests, eight_byte_bucket_creation) {
    printf("ArenaHeader size %zu \n", sizeof(ArenaHeader));
    printf("Arena size %zu \n", sizeof(Arena));
    printf("FreeResult size %zu \n", sizeof(FreeResult));
    printf("MemblockCache size %zu \n", sizeof(MemBlockCache));
    printf("Memblock size %zu \n", sizeof(MemBlock));

    Arena* arena = Arena_create(8, 1, 0);

    cr_assert_null(arena->free_stack);
    cr_assert_not_null(arena);

    ArenaHeader header = arena->header;

    cr_assert_eq(header.bucket_size, 8);
    cr_assert_eq(header.mem_pages, 1);
    cr_assert_eq(header.len, 0);
    cr_assert_eq(header.capacity, 169, "Capacity is %zu \n", header.capacity);
}

Test(arena_tests, eight_byte_multiple_pages_bucket_creation) {
    printf("ArenaHeader size %zu \n", sizeof(ArenaHeader));
    printf("Arena size %zu \n", sizeof(Arena));

    Arena* arena = Arena_create(8, 2, 0);

    cr_assert_null(arena->free_stack);
    cr_assert_not_null(arena);

    ArenaHeader header = arena->header;

    cr_assert_eq(header.bucket_size, 8);
    cr_assert_eq(header.mem_pages, 2);
    cr_assert_eq(header.len, 0);
    cr_assert_eq(header.capacity, 339, "Capacity is %zu \n", header.capacity);
}


Test(arena_tests, sixteen_byte_bucket_creation) {
    Arena* arena = Arena_create(16, 1, 0);

    cr_assert_null(arena->free_stack);
    cr_assert_not_null(arena);

    ArenaHeader header = arena->header;

    cr_assert_eq(header.bucket_size, 16);
    cr_assert_eq(header.mem_pages, 1);
    cr_assert_eq(header.len, 0);
    cr_assert_eq(header.capacity, 126, "Capacity is %zu \n", header.capacity);
}

Test(arena_tests, get_mem_blocks_between_capacity_8) {
    Arena* arena = Arena_create(8, 1, 0);
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
    Arena* arena = Arena_create(8, 1, 0);
    ArenaHeader* header = &arena->header;

    for (int i = 0; i < (int)header->capacity; i++) {
      MemBlock* block = Arena_get_mem_block(arena);
      cr_assert_not_null(block);

      *(int*)block->data = i;

      cr_assert_eq((int)header->len, i+1, "Len is %zu \n", header->len);
      cr_assert_eq(block->arena, arena, "Block arena is %p, arena is %p\n", block->arena, arena);
      cr_assert_eq(*(int*)block->data, i, "Block data is %d\n", *(int*)block->data);
    }

    // Returns null when no more space
    MemBlock* block = Arena_get_mem_block(arena);
    cr_assert_null(block);
}

Test(arena_tests, freeing_on_arena) {
    Arena* arena = Arena_create(8, 1, 0);
    MemBlock* block = Arena_get_mem_block(arena);
    FreeResult result = Arena_free_mem_block(block);

    cr_assert(result.success);
    cr_assert(result.free_stack_state == FREE_STACK_FULL);
    cr_assert_eq(arena->free_stack->len, 1);
}

Test(arena_tests, freeing_multiple_on_arena) {
    Arena* arena = Arena_create(8, 1, 0);
    MemBlock* block = Arena_get_mem_block(arena);
    MemBlock* block2 = Arena_get_mem_block(arena);
    FreeResult result = Arena_free_mem_block(block);

    cr_assert(result.success);
    cr_assert(result.free_stack_state == FREE_STACK_HAS_SPACE);

    FreeResult result2 = Arena_free_mem_block(block);

    cr_assert(result2.success);
    cr_assert(result2.free_stack_state == FREE_STACK_FULL);

    cr_assert_eq(arena->free_stack->len, 2);
}

Test(arena_tests, alloc_and_freeing_multiple_will_reuse_mem) {
    Arena* arena = Arena_create(8, 1, 0);
    MemBlock* block = Arena_get_mem_block(arena);
    FreeResult result = Arena_free_mem_block(block);

    cr_assert(result.success);
    cr_assert(result.free_stack_state == FREE_STACK_FULL);

    MemBlock* block2 = Arena_get_mem_block(arena);
    FreeResult result2 = Arena_free_mem_block(block);

    cr_assert(result2.success);
    cr_assert(result2.free_stack_state == FREE_STACK_FULL);

    cr_assert_eq(arena->free_stack->len, 1);
    cr_assert_eq(block2->data, block->data);
}

Test(arena_tests, returns_false_when_trying_to_free_more_than_available) {
    Arena* arena = Arena_create(8, 1, 0);
    MemBlock* block = Arena_get_mem_block(arena);
    FreeResult result = Arena_free_mem_block(block);

    cr_assert(result.success);
    cr_assert(result.free_stack_state == FREE_STACK_FULL);

    FreeResult result2 = Arena_free_mem_block(block);

    cr_assert(!result2.success);
    cr_assert(result2.free_stack_state == FREE_STACK_FULL);
}
