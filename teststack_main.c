#include <stdio.h>
#include <stdlib.h>

#include "teststack.h"

int main()
{
    printf("\nStack functions tests:\n\n");
    test_top_of_fun_stack();
    test_put_on_fun_stack();
    test_get_from_fun_stack();

    return EXIT_SUCCESS;
}
