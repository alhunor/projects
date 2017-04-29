#ifndef _CLASS_POINTS_H__
#define _CLASS_POINTS_H__

#include <vector>
#include "bitmap\bitmap_image.hpp"

#define UNCLASSIFIED -2
#define NOISE -3

#define CORE_POINT 1
#define NOT_CORE_POINT 0


struct point_s
{
	double x, y;
	int cluster_id;
};

double euclidean_dist(point_s *a, point_s *b);
typedef double (*distance)(point_s *a, point_s *b);

class points
{
public:
	void savetoBitmap(char* filename, unsigned int dim);
	void readxyFromFile(char* filename);
	void setPts(std::vector<point_s>& ptsInput);
	void setPts(point_s *ptsInput, int nbPointsInput);
	void clear();
	double averageDistance(int minpts);
	int numPoints() {return nbPoints;}
	point_s* getPtr() {return &pts[0];}

protected:
	void setCross(bitmap_image& image, unsigned int x, unsigned int y, int color);
	unsigned int nbPoints;
	double maxx, maxy, minx, miny;
	std::vector<point_s> pts;
};

#endif