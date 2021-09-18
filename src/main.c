#include "gmalloc.h"
#include <assert.h>

void test_first_allocation() {
  /* char* init = gmalloc_init(1); */
  /* char* a = gmalloc(sizeof(int)); */
  /* assert((*a - sizeof(AllocatedHeader)) == init); */
}

int main() {
    gmalloc_init(1);
    int* a = gmalloc(sizeof(int));

    *a = 2;

    printf("A is in %p with %d\n", a, *a);

    return 0;
}
