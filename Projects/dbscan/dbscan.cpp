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
	//p.readxyFromFile("unbalance.txt"); epsilon = 4500; minpts = 10;

	//p.readxyFromFile("gaussian(k=3, dim=2, n=90).txt"); epsilon = 2; minpts = 5;
	//p.readxyFromFile("gaussian(k=3, dim=2, n=2158).txt"); epsilon = 2; minpts = 5;
	
	//p.readxyFromFile("Aggregation (n=788, k=7, D=2).txt"); epsilon = 1.4; minpts = 6;
	
	//p.readxyFromFile("Aggregation (n=788, k=7, D=2).txt"); epsilon = 1.4; minpts = 6;
	
	//p.readxyFromFile("jain (n=373, k=2, dim=2).txt"); epsilon = 2.4; minpts = 5;
	
	//p.readxyFromFile("spiral (n=312, k=3, dim=2).txt"); epsilon = 2; minpts = 4;
	
	p.readxyFromFile("flame (n=240, k=2, dim=2).txt"); epsilon = 1; minpts = 5;
	

	double avg = p.averageDistance(6);


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