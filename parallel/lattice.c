#include "lattice.h"

/**
 * given a cluster which may be linked to other clusters, returns
 * the canonical cluster
 */
cluster *canonical(cluster *c)
{
    while (c->redirect != NULL)
    {
        c = c->redirect;
    }
    return c;
}

/**
 * specifies that two clusters are actually the same (due to
 * wraparound connections or different boxes)
 */
void merge_clusters(lattice l, cluster *a, cluster *b)
{
    //find canonical clusters
    a = canonical(a);
    b = canonical(b);

    if (a->id == b->id) return;

    //merge one with higher id into one with lower id - swap if needed
    if (a->id > b->id)
    {
        cluster *c = a;
        a = b;
        b = c;
    }

    //link b to a
    b->redirect = a;

    //increase the size of a
    a->size += b->size;

    //link the reached rows/columns of b into a
    for (int i = 0; i < l.n; ++i)
    {
        a->rows[i] |= b->rows[i];
        a->cols[i] |= b->cols[i];
    }

    //b data not needed any more - clear memory
    free(b->rows);
    free(b->cols);
}

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
 * prints out a lattice to stdout
 */
void print_lattice(lattice l)
{
  for (int i = 0; i < l.n; ++i)
  {
    for (int j = 0; j < l.n; ++j)
    {
        printf("%2d ", get_site(l, i, j)->occupied);
    }
    printf("\n");
  }
}
