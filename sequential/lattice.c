#include "lattice.h"

/**
 * returns a new empty square lattice of size n by n
 */
lattice create_lattice(int n)
{
    lattice l;
    l.n = n;
    l.sites = malloc(n * sizeof(site *));
    if (l.sites == NULL)
    {
        printf("failed to alloc\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; ++i)
    {
        l.sites[i] = calloc(n, sizeof(site));
        if (l.sites[i] == NULL)
        {
            printf("failed to alloc\n");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < n; ++j)
        {
            l.sites[i][j].i = i;
            l.sites[i][j].j = j;
            l.sites[i][j].on_stack = false;
        }
    }
    return l;
}

/**
 * returns a pointer to the site identified by (i, j) in the lattice
 */
site *get_site(lattice l, int i, int j)
{
  return l.sites[i] + j;
}

/**
 * frees up memory used by the given lattice
 */
void delete_lattice(lattice l)
{
    for (int i = 0; i < l.n; ++i)
    {
        free(l.sites[i]);
    }
    free(l.sites);
}

/**
 * returns the direction opposite to the one given
 */
int reverse(int dir)
{
    return (dir + N_DIRECTIONS / 2) % N_DIRECTIONS;
}

/**
 * returns a pointer to the neighbour of a given site in a given direction
 */
site *neighbour(lattice l, site *s, int dir)
{
    int i_offset[N_DIRECTIONS] = {-1, 0, 1, 0};
    int j_offset[N_DIRECTIONS] = {0, 1, 0, -1};
    return &(l.sites[mod_p(s->i + i_offset[dir], l.n)][mod_p(s->j + j_offset[dir], l.n)]);
}

/**
 * seeds the sites of a lattice with probability p, and forms bonds appropriately
 */
void seed_sites(lattice l, double p)
{
    for (int i = 0; i < l.n; ++i)
    {
        for (int j = 0; j < l.n; ++j)
        {
            double curr = (double) rand() / RAND_MAX;
            l.sites[i][j].occupied = curr < p;
        }
    }
    for (int i = 0; i < l.n; ++i)
    {
        for (int j = 0; j < l.n; ++j)
        {
            if (l.sites[i][j].occupied)
            {
                for (int k = 0; k < N_DIRECTIONS; ++k)
                {
                    l.sites[i][j].bond[k] = neighbour(l, get_site(l, i, j), k)->occupied;
                }
            }
        }
    }
}

/**
 * seeds the bonds of a lattice with probability p, and fills the sites appropriately
 */
void seed_bonds(lattice l, double p)
{
    for (int i = 0; i < l.n; ++i)
    {
        for (int j = 0; j < l.n; ++j)
        {
            for (int k = 0; k < 2; ++k)
            {
                double curr = (double) rand() / RAND_MAX;
                if (curr < p)
                {
                    l.sites[i][j].bond[k] = true;
                    l.sites[i][j].occupied = true;
                    site *n = neighbour(l, get_site(l, i, j), k);
                    n->occupied = true;
                    n->bond[reverse(k)] = true;
                }
            }
        }
    }
}

/**
 * prints out a lattice: n by n grid where the entries are e.g. {1, 1, 0, 1} = {N bond, E bond, no S bond, W bond}
 */
void print_lattice(lattice l)
{
  for (int i = 0; i < l.n; ++i)
  {
    for (int j = 0; j < l.n; ++j)
    {
        site s = l.sites[i][j];
        printf("{%d,%d,%d,%d} ", s.bond[0], s.bond[1], s.bond[2], s.bond[3]);
    }
    printf("\n");
  }
}
