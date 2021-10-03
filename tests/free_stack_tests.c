#include <criterion/criterion.h>

#include "../src/free_stack.h"

Test(free_stack_tests, test_basic_run) {
    PtrResult res = FreeStack_pop(NULL);
    cr_assert(IS_ERR(res));

    FreeStack* stack = FreeStack_new(1);

    res = FreeStack_pop(NULL);
    cr_expect(IS_ERR(res));

    cr_assert_eq(stack->cursor, 0);
    cr_assert_eq(stack->len, 0);
    /* printf("%zu\n", stack->capacity); */
    cr_assert_eq(stack->capacity, 508);

    int address1 = 1;
    int address2 = 2;
    int address3 = 3;

    bool succeeded = FreeStack_push(stack, (Byte*)&address1);
    cr_expect(succeeded);
    cr_expect(stack->cursor == 1);
    cr_expect(stack->len == 1);

    FreeStack_push(stack, (Byte*)&address2);
    FreeStack_push(stack, (Byte*)&address3);

    PtrResult val_res = FreeStack_pop(stack);
    cr_assert(IS_OK(val_res));
    cr_assert_eq(*val_res.the.val, 3);

    val_res = FreeStack_pop(stack);
    cr_assert(IS_OK(val_res));
    cr_assert_eq(*val_res.the.val, 2);

    val_res = FreeStack_pop(stack);
    cr_assert(IS_OK(val_res));
    cr_assert_eq(*val_res.the.val, 1);

    val_res = FreeStack_pop(stack);
    cr_assert(IS_ERR(val_res));
    cr_assert(strcmp(val_res.the.err, "Empty stack") == 0);

    bool des_ok = FreeStack_destroy(stack);
    cr_expect(des_ok);
}
