/* Copyright 2015 Gagarine Yaikhom (MIT License) */
#define  _CRT_SECURE_NO_WARNINGS
#pragma warning (disable : 4996)

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "dbscan.h"
#include "points.h"
#include "hftimer.h"


int spread2(int index, int cluster_id, point_s *points, int num_points, double epsilon, unsigned int minpts, distance dist, std::vector<int>& neighbours);
int expand2(int index, int cluster_id, point_s *points, int num_points, double epsilon, unsigned int minpts,  distance dist);

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


// compute epsilon neighbours for point "index"
void get_epsilon_neighbours2(int index, point_s *points, int num_points, double epsilon, distance dist, std::vector<int>& neighbours)
{
	for (int i = 0; i < num_points; ++i)
	{
		if (i == index) continue;
		if (dist(&points[index], &points[i]) > epsilon) continue;
		neighbours.push_back(i);
	}
}


int dbscan2(point_s *points, int num_points, double epsilon, int minpts, distance dist)
{
	int i, cluster_id = 0;

	for (i = 0; i < num_points; ++i)
	{
		if (points[i].cluster_id == UNCLASSIFIED)
		{
			if (expand2(i, cluster_id, points, num_points, epsilon, minpts, dist) == CORE_POINT)
				++cluster_id;
		}
	}
	return cluster_id;
}


int expand2(int index, int cluster_id, point_s *points, int num_points, double epsilon, unsigned int minpts,  distance dist)
{
	std::vector<int> neighbours;
	int return_value = NOT_CORE_POINT;
	get_epsilon_neighbours2(index, points, num_points, epsilon, dist, neighbours);
	if (neighbours.size() == 0) return FAILURE;
	unsigned int i;

	if (neighbours.size() < minpts)
	{
		points[index].cluster_id = NOISE;
	} else
	{
		points[index].cluster_id = cluster_id;
		for (i=0; i<neighbours.size(); ++i)
		{
			points[neighbours[i]].cluster_id = cluster_id;
		}

		i=0;
		while (i<neighbours.size())
		{
			spread2(neighbours[i], cluster_id, points, num_points, epsilon, minpts, dist, neighbours);
			++i;
		}
		return_value = CORE_POINT;
	}
	return return_value;
} // expand2(..)





int spread2(int index, int cluster_id, point_s *points, int num_points, double epsilon, unsigned int minpts, distance dist, std::vector<int>& neighbours)
{
	std::vector<int> neighbours2;
	unsigned int i;
	get_epsilon_neighbours2(index, points, num_points, epsilon, dist, neighbours2);
	if (neighbours2.size() >= minpts)
	{
		for(i =0; i<neighbours2.size(); ++i)
		{
			point_s* d = &points[neighbours2[i]];
			if (d->cluster_id == NOISE || d->cluster_id == UNCLASSIFIED)
			{
				if (d->cluster_id == UNCLASSIFIED)
				{
					neighbours.push_back(neighbours2[i]);
				}
				d->cluster_id = cluster_id;
			}
		}
	}
	return SUCCESS;
} // spread2(..)


void main(void)
{
	points p;
	double epsilon;
	int minpts;
	p.readxyFromFile("unbalance.txt"); epsilon = 4500; minpts = 10;

	//p.readxyFromFile("gaussian(k=3, dim=2, n=90).txt"); epsilon = 2; minpts = 5;
	//p.readxyFromFile("gaussian(k=3, dim=2, n=2158).txt"); epsilon = 2; minpts = 5;
	
	//double avg = p.averageDistance(4);


	int num_points = p.numPoints();
	point_s* pointPtr = p.getPtr();

	hftimer t;
	t.startTimer();
	//dbscan(pointPtr, num_points, epsilon, minpts, euclidean_dist);
	int clusters = dbscan2(pointPtr, num_points, epsilon, minpts, euclidean_dist);
	std::cout<<"Found "<<clusters<<" clusters."<<std::endl;

	std::cout<<t.getTime()<<std::endl;

	//p.setPts(points, num_points);
	p.savetoBitmap("hunor2.bmp", 256);
}