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

//info on each lattice site
typedef struct
{
    int i, j; //coordinates in lattice
    bool occupied; //whether site is open or not
    bool bond[N_DIRECTIONS]; //whether it has a bond to N E S W neighbours
    struct _cluster *cluster; //what cluster it belongs to
} site;

//info on groups of sites connected by bonds 
typedef struct _cluster
{
	int id; //unique global id

	int size; //number of sites
	bool global; //whether it leaves the bounding box of this region under consideration

	site **border_sites; //the sites that have bonds leading out the north boundary
	int n_border_sites;

	bool *rows; //whether it spans rows 0 through n - 1
	bool *cols; //whether it spans cols 0 through n - 1

	struct _cluster *redirect; //may redirect to a `canonical' cluster once merged
} cluster;

//a square lattice holding sites
typedef struct
{
    site **sites; //a 2d square array of sites
    int n; //dimensions
} lattice;

typedef struct 
{
    int il, iu; //lower and upper i
    int jl, ju; //lower and upper j
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
