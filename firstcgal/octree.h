#ifndef OCTREE_H
#define OCTREE_H

#include <iostream>
//#include <functional>
#include "heap.h"

void OctreeTest();

struct pointXYZ
{
	float x,y,z;
	double          n;              // User's unique identifier
	unsigned int    code;           // Used during octree generation
	pointXYZ() :  n(0), code(0) {};
	pointXYZ(float _x, float _y, float _z) : x(_x), y(_y), z(_z), n(0), code(0) {}
	pointXYZ(const pointXYZ& p) : x(p.x), y(p.y), z(p.z), n(p.n), code(p.code) {}
};

pointXYZ operator*(pointXYZ lhs, const float f);
std::ostream& operator<<(std::ostream& stream, const pointXYZ rhs);
pointXYZ operator+(const pointXYZ& lhs, const pointXYZ& rhs);
pointXYZ operator-(const pointXYZ& lhs, const pointXYZ& rhs);
bool operator<(const pointXYZ& rhs, const pointXYZ& lhs);

struct Bounds
{
	Bounds() {}
	Bounds(const Bounds& b) : radius(b.radius), center(b.center) {}

	Bounds(pointXYZ& _center, float _radius) : radius(_radius), center(_center) {}
	pointXYZ center;	// Center of a cubic bounding volume
	float radius;		// Radius of a cubic bounding volume
	bool inside(pointXYZ& p);
};



class Octree
{
// starting point from http://www.flipcode.com/archives/Octree_Implementation.shtml
public:
	friend class OctreeManager;
	Octree();
	Octree(unsigned int _threshold, Bounds& b);
	~Octree();

// Accessors
pointXYZ ** getPoints() const {return points;}
const unsigned int size() const {return pointCount;}

// This defines a callback for traversal
typedef bool (*callback)(const Octree &o, void *data);

bool insert(pointXYZ * const point);

static const Bounds calcCubicBounds(pointXYZ ** points, unsigned int count);
const bool traverse(callback proc, void *data) const;

protected:
	Octree* child[8];
	unsigned int pointCount;
	pointXYZ **points;
	Bounds b;
	//pointXYZ center;
	//float radius;
	unsigned int threshold;
	bool insert2(pointXYZ * const point);
	static void setcode(pointXYZ& p, const pointXYZ& centre);
	void init(); // common stuff for constructors must be a helper function prior to C++11
};

struct heapy
{
	pointXYZ * ph;
	float dist;
};

bool operator<(const heapy& rhs, const heapy& lhs);

class OctreeManager
{
public:
	OctreeManager() : threshold(5) {}
	Octree* buildOctree(pointXYZ ** points,
						const unsigned int count,
						const unsigned int _threshold,
						const unsigned int _maximumDepth);
	static pointXYZ boundsOffsetTable[8];
	typedef minHeap<heapy> HEAP;
	HEAP OctreeManager::find_knn(Octree* oct, const pointXYZ& p, int k);

protected:
	unsigned int threshold;
	unsigned int maximumDepth;

bool buildOctree2(Octree* oct, pointXYZ ** points,
							const unsigned int count,
							const Bounds &bounds,
							const unsigned int currentDepth);
};

#endif