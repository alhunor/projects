#pragma warning (disable : 4996)
#include "dbscan.h"
#include "points.h"
#include <stdio.h>
#include <stdlib.h>


node_s *create_node(int index);
int append_at_end(int index, epsilon_neighbours_s *en);
epsilon_neighbours_s *get_epsilon_neighbours(int index, point_s *points, int num_points, double epsilon, distance dist);
void print_epsilon_neighbours(point_s *points, epsilon_neighbours_s *en);
void destroy_epsilon_neighbours(epsilon_neighbours_s *en);
void dbscan(point_s *points, int num_points, double epsilon, int minpts, distance dist);
int expand(int index, int cluster_id, point_s *points,  int num_points, double epsilon, int minpts, distance dist);
int spread(int index, epsilon_neighbours_s *seeds, int cluster_id, point_s *points,  int num_points, double epsilon, int minpts, distance dist);

double adjacent_intensity_dist(point_s *a, point_s *b);
int parse_input(FILE *file, point_s **points, double *epsilon,  int *minpts);
void print_points(point_s *points, int num_points);


//#include <stdlib.h>

/*
	point_s *points;
	double epsilon;
	int minpts;
	FILE* f = fopen("example.dat", "r");
	int num_points = parse_input(f, &points);
	epsilon = 1;
	minpts = 10;

	if (num_points)
	{
		dbscan(points, num_points, epsilon, minpts, euclidean_dist);
		printf("Epsilon: %lf\n", epsilon);
		printf("Minimum points: %u\n", minpts);
		print_points(points, num_points);
	}
	fclose(f);
*/


void print_points(point_s *points, int num_points)
{
	int i = 0;
	printf("Number of points: %u\n"
		" x     y     z     cluster_id\n"
		"-----------------------------\n"
		, num_points);
	while (i < num_points)
	{
		//printf("%5.2lf %5.2lf %5.2lf: %d\n", points[i].x, points[i].y, points[i].z, points[i].cluster_id);
		printf("%5.2lf %5.2lf %5.2lf: %d\n", points[i].x, points[i].y, points[i].cluster_id);
		++i;
	}
}



int parse_input(FILE *file, point_s **points)
{
	int num_points, i = 0;
	fscanf(file, "%u\n", &num_points);
	point_s *p = (point_s *) calloc(num_points, sizeof(point_s));
	if (p == NULL)
	{
		perror("Failed to allocate points array");
		return 0;
	}
	while (i < num_points)
	{
		//fscanf(file, "%lf %lf %lf\n",&(p[i].x), &(p[i].y), &(p[i].z));
		fscanf(file, "%lf %lf %lf\n",&(p[i].x), &(p[i].y));
		p[i].cluster_id = UNCLASSIFIED;
		++i;
	}
	*points = p;
	return num_points;
}




void print_epsilon_neighbours(point_s *points,epsilon_neighbours_s *en)
{
	if (en)
	{
		node_s *h = en->head;
		while (h)
		{
//			printf("(%lfm, %lf, %lf)\n", points[h->index].x,points[h->index].y, points[h->index].z);
			printf("(%lfm, %lf, %lf)\n", points[h->index].x,points[h->index].y);
			h = h->next;
		}
	}
}


int spread(int index, epsilon_neighbours_s *seeds, int cluster_id, point_s *points, int num_points, double epsilon, int minpts, distance dist)
{
	epsilon_neighbours_s *spread = get_epsilon_neighbours(index, points, num_points, epsilon, dist);
	if (spread == NULL) return FAILURE;
	if (spread->num_members >= minpts)
	{
		node_s *n = spread->head;
		point_s *d;
		while (n)
		{
			d = &points[n->index];
			if (d->cluster_id == NOISE || d->cluster_id == UNCLASSIFIED)
			{
				if (d->cluster_id == UNCLASSIFIED)
				{
					if (append_at_end(n->index, seeds) == FAILURE)
					{
						destroy_epsilon_neighbours(spread);
						return FAILURE;
					}
				}
				d->cluster_id = cluster_id;
			}
			n = n->next;
		}
	}

	destroy_epsilon_neighbours(spread);
	return SUCCESS;
} // spread(..)


int expand(int index, int cluster_id, point_s *points, int num_points, double epsilon, int minpts,  distance dist)
{
	int return_value = NOT_CORE_POINT;
	epsilon_neighbours_s *seeds = get_epsilon_neighbours(index, points, num_points, epsilon, dist);
	if (seeds == NULL) return FAILURE;

	if (seeds->num_members < minpts)
	{
		points[index].cluster_id = NOISE;
	} else
	{
		points[index].cluster_id = cluster_id;
		node_s *h = seeds->head;
		while (h)
		{
			points[h->index].cluster_id = cluster_id;
			h = h->next;
		}

		h = seeds->head;
		while (h)
		{
			spread(h->index, seeds, cluster_id, points, num_points, epsilon, minpts, dist);
			h = h->next;
		}
		return_value = CORE_POINT;
	}
	destroy_epsilon_neighbours(seeds);
	return return_value;
} // expand(..)



void dbscan(point_s *points, int num_points, double epsilon, int minpts, distance dist)
{
	int i, cluster_id = 0;
	for (i = 0; i < num_points; ++i)
	{
		if (points[i].cluster_id == UNCLASSIFIED)
		{
			if (expand(i, cluster_id, points, num_points, epsilon, minpts, dist) == CORE_POINT)
				++cluster_id;
		}
	}
}



void destroy_epsilon_neighbours(epsilon_neighbours_s *en)
{
	if (en)
	{
		node_s *t, *h = en->head;
		while (h)
		{
			t = h->next;
			free(h);
			h = t;
		}
		free(en);
	}
}


node_s *create_node(int index)
{
	node_s *n = (node_s *) calloc(1, sizeof(node_s));
	if (n == NULL)
		perror("Failed to allocate node.");
	else
	{
		n->index = index;
		n->next = NULL;
	}
	return n;
}

int append_at_end(int index, epsilon_neighbours_s *en)
{
	node_s *n = create_node(index);
	if (n == NULL)
	{
		free(en);
		return FAILURE;
	}
	if (en->head == NULL)
	{
		en->head = n;
		en->tail = n;
	} else
	{
		en->tail->next = n;
		en->tail = n;
	}
	++(en->num_members);
	return SUCCESS;
}

epsilon_neighbours_s *get_epsilon_neighbours(int index, point_s *points, int num_points, double epsilon, distance dist)
{
	epsilon_neighbours_s *en = (epsilon_neighbours_s *) calloc(1, sizeof(epsilon_neighbours_s));
	if (en == NULL) 
	{
		perror("Failed to allocate epsilon neighbours.");
		return en;
	}
	for (int i = 0; i < num_points; ++i)
	{
		if (i == index)
			continue;
		if (dist(&points[index], &points[i]) > epsilon)
			continue;
		else
		{
			if (append_at_end(i, en) == FAILURE)
			{
				destroy_epsilon_neighbours(en);
				en = NULL;
				break;
			}
		}
	}
	return en;
}