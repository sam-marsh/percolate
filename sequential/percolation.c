#include "percolation.h"
#include "stack.h"

/**
 * n by n visited array for dfs
 */
bool **search_init(lattice l)
{
    bool **visited = malloc(l.n * sizeof(bool *));
    for (int i = 0; i < l.n; ++i)
    {
        visited[i] = calloc(l.n, sizeof(bool));
    }
    return visited;
}

/**
 * cleans up memory from search array
 */
void search_free(lattice l, bool **visited)
{
    for (int i = 0; i < l.n; ++i)
    {
        free(visited[i]);
    }
    free(visited);
}

/**
 * checks if an array is all true -- ie col/row spans lattice
 */
bool spans(bool *array, int n)
{
    for (int i = 0; i < n; ++i)
    {
        if (!array[i])
        {
            return false;
        }
    }
    return true;
}

/**
 * performs dfs from an unvisited site to find full cluster
 */
void percolate_from(lattice l, stack stack, bool **visited, bool *rows, bool *cols, site *initial, int *cluster, bool check_rows, bool *row_span, bool check_cols, bool *col_span)
{
    int tmp_cluster = 0;

    stack_push(&stack, initial);

    //loop until no more elements in cluster
    while (!stack_empty(&stack))
    {
        site *curr = stack_pop(&stack);
        int i = curr->i;
        int j = curr->j;

        if (visited[i][j])
        {
            continue;
        }

        visited[i][j] = true;

        //increase cluster size
        ++tmp_cluster;

        //this cluster spans this row and col
        rows[i] = true;
        cols[j] = true;

        //loop over potential neighbours
        for (int k = 0; k < N_DIRECTIONS; ++k)
        {
            if (curr->bond[k])
            {
                site *n = neighbour(l, curr, k);
                if (!visited[n->i][n->j] && !n->on_stack)
                {
                    //add to stack if unvisited neighbour
                    stack_push(&stack, n);
                    n->on_stack = true;
                }
            }
        }
    }

    //send back cluster size, whether it spans
    *cluster = tmp_cluster;
    *row_span = check_rows ? spans(rows, l.n) : false;
    *col_span = check_cols ? spans(cols, l.n) : false;
}

/**
 * calculates whether a lattice percolates (row, col, both), and the largest cluster
 */
bool percolation(lattice l, bool check_rows, bool check_cols, int *cluster)
{
    bool **visited = search_init(l);
    int max_cluster = 0;
    bool spans_rows = 0;
    bool spans_cols = 0;

    stack stack;
    stack_init(&stack, l.n * l.n);

    bool *rows = malloc(l.n * sizeof(bool));
    bool *cols = malloc(l.n * sizeof(bool));

    //loop over every lattice site, start dfs from all unvisited
    for (int i = 0; i < l.n; ++i)
    {
        for (int j = 0; j < l.n; ++j)
        {
            site *curr = get_site(l, i, j);
            if (curr->occupied && !visited[i][j])
            {
                //reset rows/cols
                memset(rows, 0, l.n * sizeof(bool));
                memset(cols, 0, l.n * sizeof(bool));

                //calculate cluster data
                int this_cluster;
                bool this_row_span, this_col_span;
                percolate_from(l, stack, visited, rows, cols, get_site(l, i, j), &this_cluster, check_rows, &this_row_span, check_cols, &this_col_span);
                
                //update max/spanning info
                max_cluster = MAX(max_cluster, this_cluster);
                spans_rows |= this_row_span;
                spans_cols |= this_col_span;
            }
        }
    }

    *cluster = max_cluster;
    search_free(l, visited);
    free(rows);
    free(cols);

    return (!check_rows || spans_rows) && (!check_cols || spans_cols);
}
