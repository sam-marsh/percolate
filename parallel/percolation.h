#ifndef __PERCOLATION_H
#define __PERCOLATION_H

#include <stdbool.h>
#include <math.h>
#include <omp.h>
#include "util.h"
#include "lattice.h"
#include "stack.h"

bool percolation(lattice, bool, bool, int *);

#endif
