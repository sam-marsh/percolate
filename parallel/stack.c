#include "stack.h"

/**
 * initialises a given stack to hold `max` elements at most
 */
void stack_init(stack *s, int max)
{
    s->size = 0;
    s->data = malloc(max * sizeof(site *));
    if (s->data == NULL)
    {
        printf("failed to alloc\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * adds a site to the top of the stack
 */
void stack_push(stack *s, site *d)
{
    s->data[s->size++] = d;
}

/**
 * pops a site off the top of the stack
 */
site *stack_pop(stack *s)
{
    site *tmp = s->data[s->size - 1];
    --s->size;
    return tmp;
}

/**
 * checks if the stack contains no elements
 */
bool stack_empty(stack *s)
{
    return s->size == 0;
}

/**
 * cleans up memory held by a stack
 */
void stack_free(stack *s)
{
    free(s->data);
}
