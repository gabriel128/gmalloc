#include <criterion/criterion.h>

#include "../src/free_stack.h"

Test(free_stack_tests, test_basic_run) {
    UIntResult res = FreeStack_pop(NULL);
    cr_expect(IS_ERR(res));

    FreeStack* stack = FreeStack_new();

    res = FreeStack_pop(NULL);
    cr_expect(IS_ERR(res));

    cr_expect(stack->cursor == 0);
    cr_expect(stack->len == 0);
    cr_expect(stack->capacity == 32752);

    bool succeeded = FreeStack_push(stack, 1);
    cr_expect(succeeded);
    cr_expect(stack->cursor == 1);
    cr_expect(stack->len == 1);

    FreeStack_push(stack, 2);
    FreeStack_push(stack, 3);

    UIntResult val_res = FreeStack_pop(stack);
    cr_expect(IS_OK(val_res));
    cr_expect(val_res.the.val == 3);

    val_res = FreeStack_pop(stack);
    cr_expect(IS_OK(val_res));
    cr_expect(val_res.the.val == 2);

    val_res = FreeStack_pop(stack);
    cr_expect(IS_OK(val_res));
    cr_expect(val_res.the.val == 1);

    val_res = FreeStack_pop(stack);
    cr_expect(IS_ERR(val_res));
    cr_expect(strcmp(val_res.the.err, "Empty stack") == 0);

    bool des_ok = FreeStack_destroy(stack);
    cr_expect(des_ok);
}
