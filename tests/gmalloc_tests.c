#include <criterion/criterion.h>

#include "../src/gmalloc.h"


void test_multiple_allocations(size_t size, size_t addr_diff) {
    int* mems[10];

    for(int i = 0; i < 10; i++) {
        mems[i] = gmalloc(size);
        cr_assert_not_null(mems[i]);
        *mems[i] = 1;

        if (i > 0) {
            uintptr_t prev_addr = (uintptr_t) mems[i-1];
            uintptr_t this_addr = (uintptr_t) mems[i];

            cr_expect_eq(this_addr, prev_addr+addr_diff);
        }
    }

    for(int i = 0; i < 10; i++) {
        gfree(mems[i]);
    }

    void* mems2[15];

    for(int i = 0; i < 15; i++) {
        mems2[i] = gmalloc(size);
        cr_assert_not_null(mems2[i]);

        if (i > 0 && i < 10){
            // Uses Free Stack
            uintptr_t prev_addr = (uintptr_t) mems2[i-1];
            uintptr_t this_addr = (uintptr_t) mems2[i];

            /* printf("%lx %lx %d \n", this_addr, prev_addr, addr_diff); */
            cr_assert_eq(this_addr, prev_addr-addr_diff);
        } else if (i > 10) {
            // Uses tail
            uintptr_t prev_addr = (uintptr_t) mems2[i-1];
            uintptr_t this_addr = (uintptr_t) mems2[i];

            cr_assert_eq(this_addr, prev_addr+addr_diff);
        }
    }

    for(int i = 0; i < 15; i++) {
        int x = gfree(mems2[i]);
        cr_assert_neq(x, -1);
    }
}

Test(gmalloc_tests, returns_null_when_zero_size) {
    void* a = gmalloc(0);
    cr_expect(a == NULL);
}

Test(gmalloc_tests, eight_byte_bucket_malloc_and_free) {
    int* a = gmalloc(sizeof(int));
    *a = 2;

    uintptr_t a_addr = (uintptr_t)a;

    cr_expect_eq(*a, 2);

    int free_res = gfree(a);
    cr_expect_neq(free_res, -1);

    cr_expect_eq(*a, 2);

    int* b = gmalloc(sizeof(int));
    uintptr_t b_addr = (uintptr_t)b;

    cr_expect(a_addr == b_addr);

    gfree(b);
}

Test(gmalloc_tests, alloc_and_free_multiple1) {
    test_multiple_allocations(1, 8);
}

Test(gmalloc_tests, alloc_and_free_multiple4) {
    test_multiple_allocations(4, 8);
}

Test(gmalloc_tests, alloc_and_free_multiple8) {
    test_multiple_allocations(8, 8);
}

Test(gmalloc_tests, alloc_and_free_multiple16) {
    test_multiple_allocations(16, 16);
}
Test(gmalloc_tests, alloc_and_free_multiple32) {
    test_multiple_allocations(30, 32);
}
Test(gmalloc_tests, alloc_and_free_multiple64) {
    test_multiple_allocations(55, 64);
}

Test(gmalloc_tests, alloc_and_free_multiple128) {
    test_multiple_allocations(100, 128);
}

Test(gmalloc_tests, alloc_and_free_multiple256) {
    test_multiple_allocations(200, 256);
}

Test(gmalloc_tests, alloc_too_big_is_null) {
    void* x = gmalloc(1024);
    cr_assert_null(x);
}


Test(gmalloc_tests, double_free_returns_minus_one) {
}

Test(gmalloc_tests, crossing_arenas_allocation) {
    /* for(int i = 0; i < 1000000000; i++) */
    /*     gmalloc(512); */
}

Test(gmalloc_tests, multi_threaded_allocing) {
}

Test(gmalloc_tests, arena_unmapping) {
}
