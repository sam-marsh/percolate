#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <string.h>
#include "percolation.h"
#include "lattice.h"

/**
 * user has entered wrong program args - print help message and exit
 */
void exit_incorrect_args()
{
    printf("usage: ./main lattice_size seed_prob seed_what percolation_kind\n");
    printf("\twhere seed_what=s/b for site/bond seeding, percolation_kind=0/1/2 for row/col/both percolation checking\n");
    printf("minimum lattice size is 2x2\n");
    exit(EXIT_FAILURE);
}

/**
 * main function - prints if a lattice percolates, + max cluster, time taken
 */
int main(int argc, char *argv[])
{
    srand(time(NULL));

    --argc;
    ++argv;

    if (argc < 4)
    {
        exit_incorrect_args();
    }

    int n = atoi(argv[0]);
    double p = atof(argv[1]);
    char *seed_type = argv[2];
    int percolation_type = atoi(argv[3]);

    if (n <= 0 || p < 0 || p > 1 || percolation_type < 0 || percolation_type > 2)
    {
        exit_incorrect_args();
    }

    lattice l = create_lattice(n);

    if (strcmp(seed_type, "s") == 0)
    {
        seed_sites(l, p);
    }
    else if (strcmp(seed_type, "b") == 0)
    {
        seed_bonds(l, p);
    }
    else
    {
        exit_incorrect_args();
    }

    bool row_check = percolation_type == 0 || percolation_type == 2;
    bool col_check = percolation_type == 1 || percolation_type == 2;

    int max_cluster;

    double time = omp_get_wtime();
    bool success = percolation(l, row_check, col_check, &max_cluster);

    printf("percolates=%s,max_cluster=%d,time=%.4fs\n", success ? "true" : "false", max_cluster, omp_get_wtime() - time);

    delete_lattice(l);

    return EXIT_SUCCESS;
}
