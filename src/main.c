#include "gmalloc.h"
#include <assert.h>

void test_free_stack() {
    Result res = FreeStack_pop(NULL);
    assert(IS_ERR(res));

    FreeStack* stack = FreeStack_new();

    res = FreeStack_pop(NULL);
    assert(IS_ERR(res));

    assert(stack->cursor == 0);
    assert(stack->len == 0);
    assert(stack->capacity == 32752);

    bool succeeded = FreeStack_push(stack, 1);
    assert(succeeded);
    assert(stack->cursor == 1);
    assert(stack->len == 1);

    FreeStack_push(stack, 2);
    FreeStack_push(stack, 3);

    Result val_res = FreeStack_pop(stack);
    assert(IS_OK(val_res));
    assert(val_res.the.val == 3);

    val_res = FreeStack_pop(stack);
    assert(IS_OK(val_res));
    assert(val_res.the.val == 2);

    val_res = FreeStack_pop(stack);
    assert(IS_OK(val_res));
    assert(val_res.the.val == 1);

    val_res = FreeStack_pop(stack);
    assert(IS_ERR(val_res));
    assert(strcmp(val_res.the.err, "Empty stack") == 0);

    bool des_ok = FreeStack_destroy(stack);
    assert(des_ok);
}

int main() {
    test_free_stack();

    int* a = gmalloc(sizeof(int));

    /* *a = 2; */

    /* printf("A is in %p with %d\n", a, *a); */

    return 0;
}
