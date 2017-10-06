#include "util.h"

/**
 * computes the positive mod (even if i is negative, this returns a positive #)
 */
int mod_p(int i, int n)
{
    return (i % n + n) % n;
}
