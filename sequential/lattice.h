#ifndef __LATTICE_H
#define __LATTICE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "util.h"

#define N_DIRECTIONS  4
#define NORTH         0
#define EAST          1
#define SOUTH         2
#define WEST          3

typedef struct
{
    int i, j;
    bool occupied;
    bool bond[N_DIRECTIONS];
} site;

typedef struct
{
    site **sites;
    int n;
} lattice;

lattice create_lattice(int n);
site *get_site(lattice l, int i, int j);
void delete_lattice(lattice l);
site *neighbour(lattice l, site *s, int dir);
void seed_sites(lattice l, double p);
void seed_bonds(lattice l, double p);
void print_lattice(lattice l);

#endif
