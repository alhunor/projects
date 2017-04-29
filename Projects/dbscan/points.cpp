#pragma warning (disable : 4996)
#include <fstream>
#include <vector>
#include "bitmap\bitmap_image.hpp"
#include "points.h"

using namespace std;


//http://www.partow.net/programming/bitmap/index.html


template<typename T> 
void swapValue(T& a, T& b)
{
	T c = a;
	a = b;
	b = c;
}

struct rgb{unsigned char r,g,b,alpha;};
rgb colors[6] = {{255,0,0},{0,255,0}, {0,0,255},{0,255,255}, {255,255,0},{255,0,255}};

void points::setPts(vector<point_s>& ptsInput)
{
	// copy vector and determine bounding rectangle in x and y dimensions
	nbPoints = ptsInput.size();
	if (pts.size()<nbPoints)
	{
		pts.resize(nbPoints);
	}
	for (unsigned int i=0; i<nbPoints; ++i)
	{
		pts[i] = ptsInput[i];
		if (pts[i].x>maxx) maxx = pts[i].x;
		if (pts[i].x<minx) minx = pts[i].x;
		if (pts[i].y>maxy) maxy = pts[i].y;
		if (pts[i].y<miny) miny = pts[i].y;
	}
} // void points::setPts(vector<point_s>& ptsInput)


void points::setPts(point_s *ptsInput, int nbPointsInput)
{
	nbPoints = nbPointsInput;
	if (pts.size()<nbPoints)
	{
		pts.resize(nbPoints);
	}
	// copy points and compute bounding rectangle
	maxx = maxy = -9999999;
	minx = miny = 9999999;
	for (unsigned int i=0; i<nbPoints; ++i)
	{
		pts[i] = ptsInput[i];
		if (pts[i].x>maxx) maxx = pts[i].x;
		if (pts[i].x<minx) minx = pts[i].x;
		if (pts[i].y>maxy) maxy = pts[i].y;
		if (pts[i].y<miny) miny = pts[i].y;
	}
} // void points::setPts(point_s *ptsInput, int nbPointsInput)

void points::savetoBitmap(char* outputFile, unsigned int dim)
{
	if (nbPoints<=0) return;

	bitmap_image image(dim+1,dim+1);
	unsigned int i, j;
	// Fill bitmap with white background
	// TODO XXX - check if bitmap has a method to clear background more efficiently
	for (i = 0; i < dim; ++i)
	{
		for (j = 0; j < dim; ++j)
		{
			image.set_pixel(i, j, 255, 255, 255);
		}
	}
	// (minx, miny, maxx, maxy) bounding rectabgle must be set  before
	// scale image to bitmap dimesion dim. We leave 10 empty pixels all around
	double x, y;
	for (unsigned int i=0; i<nbPoints; ++i)
	{
		x = 10+(pts[i].x - minx) * (dim-20) / (maxx-minx);
		y = dim - (10+(pts[i].y - miny) * (dim-20) / (maxy-miny));
		setCross(image, (unsigned int)x, (unsigned int)y, pts[i].cluster_id); // sets a little cross
		//image.set_pixel((int)x, (int)y, 255, 0, 0);
	}
	image.save_image(outputFile);
} // void SavePointtoBitmap(char* outputFile)


void points::readxyFromFile(char* filename)
{
	ifstream in;
	in.open(filename);
	// XXX TODO check for file exists
	if (in.bad()) 
	{
		nbPoints = 0;
		return;
	}


	in>>nbPoints;
	pts.resize(nbPoints);
	point_s point;
	// determine bounding rectangle in x and y dimensions
	maxx = maxy = -9999999;
	minx = miny = 9999999;
	for (unsigned int i=0; i<nbPoints; ++i)
	{
		in>>point.x>>point.y;
		point.cluster_id = UNCLASSIFIED;
		//point.z = 0;

		if (point.x>maxx) maxx = point.x;
		if (point.x<minx) minx = point.x;
		if (point.y>maxy) maxy = point.y;
		if (point.y<miny) miny = point.y;
		//point.z = 0;
		point.cluster_id = UNCLASSIFIED;
		pts[i] = point;
	}
	in.close();
} // void points::readxyFromFile(char* filename)

void points::setCross(bitmap_image& image, unsigned int x, unsigned int y, int color)
{
	if (color < 0) 
	{
		// noise is shown as black dot
		image.set_pixel(x, y, 0, 0, 0);
		return;
	}
	image.set_pixel(x-2, y, colors[color].r, colors[color].g, colors[color].b);
	image.set_pixel(x-1, y, colors[color].r, colors[color].g, colors[color].b);
	image.set_pixel(x+1, y, colors[color].r, colors[color].g, colors[color].b);
	image.set_pixel(x+2, y, colors[color].r, colors[color].g, colors[color].b);
	image.set_pixel(x, y, colors[color].r, colors[color].g, colors[color].b);
	image.set_pixel(x, y-1, colors[color].r, colors[color].g, colors[color].b);
	image.set_pixel(x, y-2, colors[color].r, colors[color].g, colors[color].b);
	image.set_pixel(x, y+1, colors[color].r, colors[color].g, colors[color].b);
	image.set_pixel(x, y+2, colors[color].r, colors[color].g, colors[color].b);
} // void points::setCross(bitmap_image& image, unsigned int x, unsigned int y, unsigned int color)

void points::clear()
{
	nbPoints = 0;
	// pts.resize(0);
}

double points::averageDistance(int minpts)
{
	int num_points = numPoints();
	point_s* pointPtr = getPtr();
	
	if (num_points<=0 || pointPtr==NULL)
	{
		cout<<"Invalid data set."<<endl;
		return 0;
	}

	// compute average distance between points
	int i,j, k;
	int* neighbours = new  int [minpts+1]; 
	double* distances =  new double [minpts+1];

	int nbNeighbours;
	double maxDist = 99999, dist;
	int candidate;
	double avg = 0;

	for (i=0; i<num_points; ++i)
	{
		nbNeighbours = 1;
		neighbours [0]= 0;
		maxDist = euclidean_dist(&pointPtr[0], &pointPtr[i]);
		distances[0] = maxDist;
		for (j=0; j<num_points; ++j)
		{
			if (i!=j)
			{
				dist = euclidean_dist(&pointPtr[i], &pointPtr[j]);
				if (dist < maxDist)
				{
					candidate = j;
					// insert the point in neighbours, maintain sorted order
					for (k=0; k<nbNeighbours; ++k)
					{
						if (dist<distances[k])
						{
							swapValue(candidate, neighbours[k]);
							swapValue(dist, distances[k]);
						}
					} // for (k=0; k<nbNeighbours; ++k)
					if (nbNeighbours < minpts)
					{
						distances[nbNeighbours] = dist;
						neighbours[nbNeighbours] = candidate;
						++nbNeighbours;
					}
				} else if (nbNeighbours<minpts)
				{
					distances[nbNeighbours] = dist;
					neighbours[nbNeighbours] = j;
					maxDist = dist;
					++nbNeighbours;
				}
			}
		} // for (j=0; j<num_points; ++j)
		// compute average diatance
		for (k=0; k<minpts; ++k)
		{
			avg += distances[k];
		}
	} // for (i=0; i<num_points; ++i)
	delete[] neighbours; 
	delete[]  distances;
	return avg / (minpts*num_points);
} // void points::averageDistance(int minpts)



inline double sqr(double x) {return x*x;}

double euclidean_dist(point_s *a, point_s *b)
{
	//return sqrt(sqr(a->x - b->x) + sqr(a->y - b->y) + sqr(a->z - b->z));
	return sqrt(sqr(a->x - b->x) + sqr(a->y - b->y) );
}
