#ifndef __STACK_H
#define __STACK_H

#include <stdbool.h>
#include "lattice.h"

typedef struct {
    site **data;
    int size;
} stack;

void stack_init(stack *s, int max);
void stack_push(stack *s, site *d);
site *stack_pop(stack *s);
bool stack_empty(stack *s);
void stack_free(stack *s);

#endif
