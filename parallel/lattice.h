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

#define BOX_WIDTH(b) ((b).ju - (b).jl + 1)
#define BOX_HEIGHT(b) ((b).iu - (b).il + 1)

typedef struct
{
    int i, j;
    bool occupied;
    bool bond[N_DIRECTIONS];
    struct _cluster *cluster;
} site;

typedef struct _cluster
{
	int id;

	int size;
	bool global;

	site **border_sites;
	int n_border_sites;

	bool *rows;
	bool *cols;

	struct _cluster *redirect;
} cluster;

typedef struct
{
    site **sites;
    int n;
} lattice;

typedef struct 
{
    int il, iu;
    int jl, ju; 
} box;

lattice create_lattice(int n);
site *get_site(lattice l, int i, int j);
void delete_lattice(lattice l);
site *neighbour(lattice l, site *s, int dir);
void seed_sites(lattice l, double p);
void seed_bonds(lattice l, double p);
void print_lattice(lattice l);
cluster *canonical(cluster *c);
void merge_clusters(lattice l, cluster *a, cluster *b);

#endif
