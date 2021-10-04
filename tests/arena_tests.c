#include <criterion/criterion.h>

#include <stddef.h>

#include "../src/arena.h"

Test(arena_tests, eight_byte_bucket_creation) {
    /* printf("ArenaHeader size %zu \n", sizeof(ArenaHeader)); */
    /* printf("Arena size %zu \n", sizeof(Arena)); */
    /* printf("Offset of blocks %zu \n", offsetof(Arena, next_arena)); */

    Arena* arena = Arena_create(8, 1);

    cr_assert_not_null(arena->free_stack);
    cr_assert_not_null(arena);
    cr_assert_null(arena->next_arena);
    cr_assert_null(arena->prev_arena);

    ArenaHeader header = arena->header;

    cr_assert_eq(header.bucket_size, 8);
    cr_assert_eq(header.mem_pages, 1);
    cr_assert_eq(header.len, 0);
    cr_assert_eq(header.capacity, 506, "Capacity is %zu \n", header.capacity);


}

Test(arena_tests, sixteen_byte_bucket_creation) {
    /* Arena* arena = Arena_create(16); */

    /* ArenaHeader* header = arena->header; */

    /* cr_assert_eq(header->bucket_size, 16); */
    /* cr_assert_eq(header->pages, 1); */
    /* cr_assert_not_null(header->free_stack); */
    /* cr_assert_eq(header->capacity, 252, "Capacity is %zu \n", header->capacity); */
}
