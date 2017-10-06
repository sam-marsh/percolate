#ifndef __PERCOLATION_H
#define __PERCOLATION_H

#include <string.h>
#include "lattice.h"

bool percolation(lattice l, bool row_check, bool col_check, int *cluster);

#endif
