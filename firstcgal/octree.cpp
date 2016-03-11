#include "Octree.h"

#include <fstream>
#include <iostream>
#include <conio.h>
#include "heap.h"


using namespace std;


pointXYZ operator*(pointXYZ lhs, const float f)
{
	pointXYZ p;
	p.x= lhs.x*f;
	p.y= lhs.y*f;
	p.z= lhs.z*f;
	return p;
};

ostream& operator<<(ostream& stream, const pointXYZ rhs)
{
	stream<<rhs.x<<" "<<rhs.y<<" "<<rhs.z;
	return stream;
}

pointXYZ operator+(const pointXYZ& lhs, const pointXYZ& rhs)
{
	pointXYZ p;
	p.x= lhs.x+rhs.x;
	p.y= lhs.y+rhs.y;
	p.z= lhs.z+rhs.z;
	return p;
}

pointXYZ operator-(const pointXYZ& lhs, const pointXYZ& rhs)
{
	pointXYZ p;
	p.x= lhs.x - rhs.x;
	p.y= lhs.y - rhs.y;
	p.z= lhs.z - rhs.z;
	return p;
}

bool operator<(const pointXYZ& lhs, const pointXYZ& rhs){return lhs.x<rhs.x;}

// These offsets determine where a node is relative to it's parent. For example, the 
//bottom-left-rear (-x, -y, -z) child for a node is encoded by (-1, -1, -1).
// However, since the radius of a child is always half of its parent's, we use a table of 0.5, rather than 1.0 for efficiency.
pointXYZ OctreeManager::boundsOffsetTable[8] =
{
	pointXYZ(-0.5, -0.5, -0.5),
	pointXYZ(+0.5, -0.5, -0.5),
	pointXYZ(-0.5, +0.5, -0.5),
	pointXYZ(+0.5, +0.5, -0.5),
	pointXYZ(-0.5, -0.5, +0.5),
	pointXYZ(+0.5, -0.5, +0.5),
	pointXYZ(-0.5, +0.5, +0.5),
	pointXYZ(+0.5, +0.5, +0.5)
};

bool operator<(const heapy& lhs, const heapy& rhs)
{
	return lhs.dist < rhs.dist;
}

bool Bounds::inside(pointXYZ& p)
{
	// alternatively it is more efficient to compare L_inf norm of p-centre to radius
	if (p.x < center.x-radius || p.x > center.x+radius) return false;
	if (p.y < center.y-radius || p.y > center.y+radius) return false;
	if (p.z < center.z-radius || p.x > center.z+radius) return false;
	return true;
}


Octree::Octree(unsigned int _threshold, Bounds& _b) : pointCount(0), b(_b), threshold(_threshold)
{
	init();
}

void Octree::init()
{
	memset(child, 0, sizeof(child));
	points = new pointXYZ*[threshold];
}

Octree::~Octree() 
{
	int i;
	for (i=0; i<8; ++i)
	{
		if (!child[i]) delete child[i];
	}
	delete[] points;
}


void Octree::setcode(pointXYZ& p, const pointXYZ& center)
{
	p.code = 0;
	if (p.x > center.x) p.code |= 1;
	if (p.y > center.y) p.code |= 2;
	if (p.z > center.z) p.code |= 4;
}


Octree* OctreeManager::buildOctree(pointXYZ **points,
							const unsigned int count,
							const unsigned int _threshold,
							const unsigned int _maximumDepth)
{
	threshold = _threshold;
	maximumDepth = _maximumDepth;

	Bounds b = Octree::calcCubicBounds(points, 1000);
	Octree* oct = new Octree(threshold, b);

//	cout<<b.center<<"   "<<b.radius<<endl;

	buildOctree2(oct, points, count, b, 0);
	return oct;
}


bool OctreeManager::buildOctree2(Octree* oct, pointXYZ ** const _points,
							const unsigned int count,
							const Bounds &bounds,
							const unsigned int currentDepth)
{ // called from OctreeManager::buildOctree
	cout<<" Level: "<<currentDepth<<"  Count   "<<count<<endl;
	unsigned int i, j;

	// We specifically use ">=" for the depth comparison so that we can set the maximumDepth depth to 0 if we want a tree with no depth.
	if (count <= threshold || currentDepth >= maximumDepth)
	{
		// Store the points in the leaf
		oct->pointCount = count;
		for (i=0; i<count; ++i) oct->points[i] = _points[i];
		return true;
	}

	unsigned int childPointCounts[8];
	for (i = 0; i < 8; i++) childPointCounts[i] = 0;

	// Assign each point to a child node
	for (i = 0; i < count; i++)
	{
		pointXYZ &p = *_points[i];

		// Build an index into the child[] array using the relative position of the point to the center of the current node

		Octree::setcode(p, bounds.center);

		// count points for each child
		childPointCounts[p.code]++;
	}
	
	// Allocate a list of points that were coded JUST for this child
	pointXYZ **newList[8];
	unsigned int newCounts[8];
	for (i = 0; i < 8; i++)
	{
		// Allocate a list of points that were coded JUST for this child
		newList[i] = new pointXYZ *[childPointCounts[i]];
		newCounts[i] = 0;
	}

	// Go through the input list of points and copy over the points that were coded for this child
	int code;
	for (j = 0; j < count; j++)
	{
		code = _points[j]->code;
		newList[code][newCounts[code]++] = _points[j];
	}


	// Recursively call build() for each of the 8 children
	for (i = 0; i < 8; i++)
	{
		// Don't bother going any further if there aren't any points for this child

		if (!childPointCounts[i]) continue;

		// Create a new Bounds, with the center offset and half the radius
		Bounds  newBounds;
		newBounds.radius = bounds.radius * 0.5f;
		newBounds.center = bounds.center + boundsOffsetTable[i] * bounds.radius;

		oct->child[i] = new Octree(threshold, newBounds); // Allocate the child

		// Recurse
		buildOctree2(oct->child[i], newList[i], newCounts[i], newBounds, currentDepth+1);
	}

	for (i = 0; i < 8; i++)
	{
		delete[] newList[i];
	}

	return true;
} // void OctreeManager::buildOctree2(pointXYZ ** const points, ..)



bool Octree::insert(pointXYZ * const p)
{
// return true if insertion succeded, false otherwise
	if (!b.inside(*p)) return false;
	return insert2(p);
} // bool Octree::insert(pointXYZ * const point)


bool Octree::insert2(pointXYZ * const p)
{
	unsigned int i;
	int code;

	// return true if insertion succeded, false otherwise
	// called by Octree::insert once it has checked that the point is inside bounds, does not check bounds 
	if (pointCount < threshold)
	{ //can fit in current leaf
		points[pointCount++] = p;
		return true;
	}
	else // transform leaf to inernal node then isert existing and new points in children
	{
		setcode(*p, b.center);
		if (!points)
		{ // has been converted to internal node
			code = p->code;
			if (child[code]==NULL)
			{
				Bounds  newBounds;
				newBounds.radius = b.radius * 0.5f;
				newBounds.center = b.center + OctreeManager::boundsOffsetTable[code] * b.radius;
				child[code] = new Octree(threshold, newBounds);
			}
			child[code]->insert2(p);
			return true;
		}
		for (i=0; i<threshold; ++i)
		{
			setcode(*points[i], b.center);
		}
		// insert existing points
		for (i = 0; i < pointCount; i++)
		{
			code = points[i]->code;
			if (child[code]==NULL)
			{
				Bounds newBounds;
				newBounds.radius = b.radius * 0.5f;
				newBounds.center = b.center + OctreeManager::boundsOffsetTable[code] * b.radius;
				child[code] = new Octree(threshold, newBounds);
			}
			child[code]->insert2(points[i]);
		}
		// insert new point
		code = p->code;
		if (child[code]==NULL)
		{
			Bounds newBounds;
			newBounds.radius = b.radius * 0.5f;
			newBounds.center = b.center + OctreeManager::boundsOffsetTable[code] * b.radius;
			child[code] = new Octree(threshold, newBounds);
		}
		child[code]->insert2(p);

		delete[] points;
		points=NULL;
		return true;
	}
} // bool Octree::insert(pointXYZ * const point)


// Determine the [cubic] bounding volume for a set of points
const Bounds Octree::calcCubicBounds(pointXYZ ** points, unsigned int count)
{
	unsigned int i;
	Bounds  b;

	// Determine min/max of the given set of points

	pointXYZ  min = *points[0];
	pointXYZ  max = *points[0];

	for (i = 1; i < count; i++)
	{
		const pointXYZ& p = *points[i];
		if (p.x < min.x) min.x = p.x;
		if (p.y < min.y) min.y = p.y;
		if (p.z < min.z) min.z = p.z;
		if (p.x > max.x) max.x = p.x;
		if (p.y > max.y) max.y = p.y;
		if (p.z > max.z) max.z = p.z;
	}

	pointXYZ   radius = max - min;
	b.center = min + radius * 0.5f; // center of this space

	// radius should contains the entire CUBIC volume.
	b.radius = radius.x;
	if (b.radius < radius.y) b.radius = radius.y;
	if (b.radius < radius.z) b.radius = radius.z;

	return b;
} // const Bounds Octree::calcCubicBounds(const pointXYZ ** points, const unsigned int count)




// Generic tree traversal
const bool Octree::traverse(callback proc, void *data) const
{
	unsigned int i;

	// Call the callback for this node (if the callback returns false, then stop traversing.
	if (!proc(*this, data)) return false;

	// Recursively traverse children
	if (!pointCount)
	{
		for (i = 0; i < 8; i++)
		{
			if (!child[i]) continue;
			if (!child[i]->traverse(proc, data)) return false;
		}
	}
	return true;
} // const bool Octree::traverse(callback proc, void *data) const



OctreeManager::HEAP OctreeManager::find_knn(Octree* oct, const pointXYZ& p, int k)
{ // returns "k" nearest neighbours of "p"
//	oct->setcode(p, oct->b.center);
	return 0;
} //bool OctreeManager::find_knn(Octree* oct, const pointXYZ& p, int k)

void OctreeTest()
{
int i;

	minHeap<pointXYZ> mh;

	char* filename = "shperecloud.csv";
	ifstream is;
	is.open(filename);
	int N;
	is>>N;
	pointXYZ* points = new pointXYZ[N];
	pointXYZ** pl = new pointXYZ*[N];
	for (i=0; i<N; ++i)
	{
		is>>points[i].x>>points[i].y>>points[i].z;
		pl[i]=&points[i];
		mh.push(points[i]);
	}
	is.close();

	OctreeManager om;
//	Octree* o = om.buildOctree(pl,1000,5,64);
	
	Bounds b = Octree::calcCubicBounds(pl, 1000);
	Octree o2(5, b);

	for (i=0;i<1000; ++i)
	{
		//om.insert(o2, pl[i]);
	//	o2.insert(pl[i]);
//		cout<<i<<endl;
	}

	while (! mh.empty())
	{
	
		cout<<mh.popTop()<<endl;
	}

}