#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fun_stack.h"

struct funstack *first;

int top_of_funstack(void)
{
    if(first != NULL) return first->par_level;

    else return 1;
}

void put_on_fun_stack( int par_level, char *funame )
{
    struct funstack *stack = NULL;
    stack = malloc(sizeof (struct funstack));

    stack->funame = (char*) malloc((strlen(funame)+1) * sizeof(stack->funame)); //dodaje jedno miejsce dla znaku białego
    strcpy(stack->funame, funame);

    stack->par_level = par_level;

    stack->previous = first;

    first = stack;
}

char *get_from_fun_stack(void)
{
    if(first == NULL) return 1;

    char *temp;

    temp = first->funame;
    first = first->previous;

    return temp;
}