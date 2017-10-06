#include "util.h"

/**
 * computes the positive mod (even if i is negative, this returns a positive #)
 */
int mod_p(int i, int n)
{
    return (i % n + n) % n;
}

/**
 * checks if all the elements in a boolean array are true
 */
bool all(bool *b, int n)
{
	for (int i = 0; i < n; ++i)
		if (!b[i]) return false;
	return true;
}