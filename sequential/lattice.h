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

//info on each lattice site
typedef struct
{
    int i, j; //coordinates in lattice
    bool occupied; //whether site is open or not
    bool bond[N_DIRECTIONS]; //whether it has a bond to N E S W neighbours
    bool on_stack; //make dfs more space efficient by only adding to stack once
} site;

//a square lattice holding sites
typedef struct
{
    site **sites; //a 2d square array of sites
    int n; //dimensions
} lattice;

lattice create_lattice(int n);
site *get_site(lattice l, int i, int j);
void delete_lattice(lattice l);
site *neighbour(lattice l, site *s, int dir);
void seed_sites(lattice l, double p);
void seed_bonds(lattice l, double p);
void print_lattice(lattice l);

#endif
