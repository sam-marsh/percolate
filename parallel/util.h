#ifndef __UTIL_H
#define __UTIL_H

#include <stdbool.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int mod_p(int i, int n);

bool all(bool *b, int n);

#endif
