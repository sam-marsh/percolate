#include "percolation.h"

/**
 * starting from an initial site, performs a depth first search, marking
 * all visited sites as part of the same cluster
 *
 * only searches within the specified region
 */
void explore_cluster(lattice l, site *initial, box b, stack *stack, bool **visited, cluster *c)
{
	//push the first site onto the cluster
	stack_push(stack, initial);

	//continue until no sites left in this box which are connected to the initial site
	while (!stack_empty(stack))
	{
		//look at the next site
		site *s = stack_pop(stack);
		int i = s->i;
		int j = s->j;

		if (visited[i - b.il][j - b.jl])
		{
			continue;
		}

		visited[i - b.il][j - b.jl] = true;

		//add one to the cluster size
		++c->size;

		//allocate the cluster to the site
		s->cluster = c;

		if ((i == b.il && s->bond[NORTH]) || (i == b.iu && s->bond[SOUTH]))
		{
			if (i == b.il && s->bond[NORTH])
			{
				//site is on the N edge and has bond out of cluster - keep track
				c->border_sites[c->n_border_sites++] = s;
			}

			//site has a connection to outside the box -- mark cluster global
			c->global = true;
		}

		//cluster has reached this row and this column
		c->rows[i] = true;
		c->cols[j] = true;

		for (int d = 0; d < N_DIRECTIONS; ++d)
		{
			//neighbour in this direction
			if (s->bond[d])
			{
				//ignore if outside local box
				if ((i == b.il && d == NORTH) || (i == b.iu && d == SOUTH))
				{
					continue;
				}

				site *n = neighbour(l, s, d);
				if (!visited[n->i - b.il][n->j - b.jl])
				{
					//add onto stack if not visited yet
					stack_push(stack, n);
				}
			}
		}
	}
}

/**
 * given a region to search in, finds clusters within the region,
 * and returns the clusters which leave the north border to the caller 
 */
cluster **find_global_clusters(lattice l, int initial_id, box b, int *max_size, int *n_clusters)
{
	//allocate list with max number of clusters
	cluster **clusters = malloc(BOX_WIDTH(b) / 2 * sizeof(cluster *));
	//number of clusters
	int count = 0;
	//max cluster size in this region
	int max = 0;

	//allocate 2D array to keep track of visited sites in the DFS
	bool **visited = malloc(BOX_HEIGHT(b) * sizeof(bool *));
	for (int i = 0; i < BOX_HEIGHT(b); ++i)
	{
		visited[i] = calloc(BOX_WIDTH(b), sizeof(bool));
	}

	//use stack for DFS data structure
	stack stack;
	stack_init(&stack, BOX_WIDTH(b) * BOX_HEIGHT(b));

	//all sites in region
	for (int i = b.il; i <= b.iu; ++i)
	{
		for (int j = b.jl; j <= b.ju; ++j)
		{
			site *s = get_site(l, i, j);

			//check if contains a site, and also hasn't been reached by DFS yet
			if (s->occupied && !visited[i - b.il][j - b.jl])
			{
				//new site -- create and set up a new cluster
				cluster *c = calloc(1, sizeof(cluster));
				c->id = initial_id;
				c->border_sites = malloc(BOX_WIDTH(b) * sizeof(site *));
				c->rows = calloc(l.n, sizeof(bool));
				c->cols = calloc(l.n, sizeof(bool));

				//find other sites in the cluster, fill in cluster data
				explore_cluster(l, s, b, &stack, visited, c);

				//update max cluster size
				max = MAX(max, c->size);

				//if cluster reaches outside box, record it, otherwise delete
				if (c->global)
				{
					clusters[count++] = c;
					initial_id++;
				}
				else
				{
					free(c->border_sites);
					free(c->rows);
					free(c->cols);
					free(c);
				}
			}
		}
	}

	free(visited);

	*max_size = max;
	*n_clusters = count;

	return clusters;
}

/**
 * perform percolation analysis on the given lattice
 * pass in a lattice, whether to check rows, cols, how many threads in use
 * returns whether lattice percolates, and the size of the max cluster
 */
bool percolation(lattice l, bool rows, bool cols, int *max_cluster)
{
	int num_threads;
	#pragma omp parallel
	num_threads = omp_get_num_threads();

	//use n boxes for n threads
	int n_boxes = MIN(num_threads, l.n);
	int extra = l.n % n_boxes;

	//the max over all clusters, of all boxes
	int full_max = 0;

	//allocate num clusters, cluster list, max cluster size for each box
	int *box_n = malloc(n_boxes  * sizeof(int));
	cluster ***box_clusters = malloc(n_boxes  * sizeof(cluster **));
	int *maxes = calloc(n_boxes , sizeof(int));

	//bounding and labelling info for boxes
	int start_label = 1;
	int il = 0;
	box boxes[n_boxes];
	int start_labels[n_boxes];

	//fill in box info - very inexpensive, very small loop. don't need to parallelise
	for (int id = 0; id < n_boxes; ++id)
	{
		//create bounds for overlay lattice box
		box b;
		b.il = il;
		b.iu = il + l.n / n_boxes - 1;
		b.jl = 0;
		b.ju = l.n - 1;
		
		//if doesn't evenly divide n, add 1 more row if leftover
		if (id % n_boxes < extra)
		{
			++b.iu;
		}

		//store info on this box into array
		boxes[id] = b;
		start_labels[id] = start_label;

		//update il and jl for next overlay box
		il = b.iu + 1;

		//increment start label to keep unique amongst boxes
		start_label += BOX_WIDTH(b) * BOX_HEIGHT(b) / 2;
	}

	//parallel speedup comes here! process boxes on different threads
	#pragma omp parallel
	#pragma omp single
	for (int id = 0; id < n_boxes; ++id)
	{
		//submit a new task for this box
		#pragma omp task firstprivate(id)
		{
			int box_max;
			int n_clusters;

			//calculate max cluster, all global clusters for box
			cluster **clusters = find_global_clusters(l, start_labels[id], boxes[id], &box_max, &n_clusters);
			
			//store important info in array
			box_n[id] = n_clusters;
			box_clusters[id] = clusters;
			maxes[id] = box_max;
		}
	}

	//completed processing of boxes, now need to patch together
	//can't get speedup with parallelism here due to required synchronisation
	//look at each box in turn
	for (int i = 0; i < n_boxes; ++i)
	{
		//check if max can be increased here, while doing this patching
		full_max = MAX(full_max, maxes[i]);

		//iterate over global clusters in box (north side)
		for (int j = 0; j < box_n[i]; ++j)
		{
			cluster *c = box_clusters[i][j];

			//loop over border sites of this cluster
			for (int k = 0; k < c->n_border_sites; ++k)
			{
				site *s = c->border_sites[k];

				if (s->bond[NORTH])
				{
					site *n = neighbour(l, s, NORTH);

					//if different cluster ids, merge them
					if (n->cluster != NULL && canonical(c)->id != canonical(n->cluster)->id)
					{
						merge_clusters(l, c, n->cluster);
					}
				}
			}
		}
	}

	//if need to check for row percolation, set to false, otherwise true
	bool row_percolation = !rows;

	//likewise for columns
	bool col_percolation = !cols;

	//can parallelise this -- just summing up/reduction
	#pragma omp parallel for reduction(max:full_max)
	for (int i = 0; i < n_boxes; ++i)
	{
		for (int j = 0; j < box_n[i]; ++j)
		{
			cluster *c = box_clusters[i][j];

			if (c->redirect == NULL)
			{
				//if could potentially be row spanning, check and update if required
				if (!row_percolation && c->size > l.n) row_percolation = all(c->rows, l.n);
				
				//likewise for columns
				if (!col_percolation && c->size > l.n) col_percolation = all(c->cols, l.n);
				
				//update the max cluster size once merged global clusters
				full_max = MAX(full_max, c->size);
			}
		}
	}

	*max_cluster = full_max;

	return row_percolation && col_percolation;
}