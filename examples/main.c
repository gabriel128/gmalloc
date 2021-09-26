#include "../src/gmalloc.h"
int main() {
    int* a = gmalloc(sizeof(int));
    *a = 2;

    printf("A is in %p with %d\n", a, *a);

    gfree(a);

    int* b = gmalloc(sizeof(int));
    *b = 2;
    printf("B is in %p with %d\n", b, *b);
    gfree(b);
    gfree(b);

    return 0;
}
