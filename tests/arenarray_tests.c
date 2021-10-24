#include <criterion/criterion.h>

#include <stddef.h>

#include "../src/arenarray.h"
#include <signal.h>

Test(arenarray_tests, arenarray_creation) {
    printf("ArenarrayItem %zu \n", sizeof(ArenarrayItem));
    printf("Arenarray size %zu \n", sizeof(Arenarray));

    Arenarray arenarray = Arenarray_new(8);

    cr_assert_not_null(arenarray.arenas[0].arena);
    cr_assert(arenarray.arenas[0].used);
    cr_assert(!arenarray.arenas[0].is_full);
}

Test(arenarray_tests, arenarray_find_arena_with_space) {
    Arenarray arenarray = Arenarray_new(8);

    Arena* arena = Arenarray_arena_with_space(&arenarray);
    cr_assert_not_null(arena);
    cr_assert_eq(arena, arenarray.arenas[0].arena);
}

Test(arenarray_tests, arenarray_find_arena_with_space2) {
    Arenarray arenarray1 = Arenarray_new(8);
    arenarray1.arenas[0].is_full = true;

    Arena* arena = Arenarray_arena_with_space(&arenarray1);
    cr_assert(arenarray1.arenas[1].used);
    cr_assert_eq(arena, arenarray1.arenas[1].arena);
}

Test(arenarray_tests, gets_mem_blocks_when_inside_capacity8) {
    Arenarray arenarray = Arenarray_new(8);
    Arena* arena = arenarray.arenas[0].arena;
    ArenaHeader* header = &arena->header;

    for (int i = 0; i < (int)header->capacity; i++) {
      MemBlock* block = Arenarray_find_mem_block(&arenarray);
      cr_assert_not_null(block);

      *(int*)block->data = i;
    }

    // It doesn't returns null when no more space
    MemBlock* block = Arenarray_find_mem_block(&arenarray);
    cr_assert_not_null(block);
    cr_assert_not_null(arenarray.arenas[1].arena);
    cr_assert_null(arenarray.arenas[2].arena);
}

Test(arenarray_tests, returns_null_when_full) {
    Arenarray arenarray = Arenarray_new(8);

    for(int i = 0; i < ARENARRAY_LEN; i++) {
        arenarray.arenas[i].used = true;
        arenarray.arenas[i].is_full = true;
    }

    MemBlock* block = Arenarray_find_mem_block(&arenarray);
    cr_assert_null(block);
}

/* Test(arenarray_tests, full_is_false_after_freeing_mem) { */
/* } */
