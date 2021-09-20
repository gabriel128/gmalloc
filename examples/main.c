#include "../src/gmalloc.h"
int main() {
    int* a = gmalloc(sizeof(int));

    /* *a = 2; */

    /* printf("A is in %p with %d\n", a, *a); */

    return 0;
}
