#include <criterion/criterion.h>

#include "../src/arena.h"

Test(arena_tests, eight_byte_bucket_creation) {
    Arena* arena = Arena_create(8);

    ArenaHeader* header = arena->header;

    cr_assert_eq(header->bucket_size, 8);
    cr_assert_eq(header->pages, 1);
    cr_assert_not_null(header->free_stack);
    cr_assert_eq(header->capacity, 10);
    /* cr_assert_eq(header->bucket_size, 8); */
}
