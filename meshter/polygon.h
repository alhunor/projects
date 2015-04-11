#ifndef POLYGON_H_
#define POLYGON_H_

#include "vector.h"
#include "point.h"
#include "quadtree.h"
//#include <list>
#include <stack>

#include <cmath>


struct myVertex
{
	myVertex() : prev(NULL), next(NULL) {}
	void clear();
	myVertex *prev, *next;
	bool inserted; // set to true if the vertex has been inserted after an edge was split
	MyREAL outgoingEdgeLen;
	MyREAL angle;
	int pointID;
///	void split(double p, bool after); 
	
	// this operator will be used by std algorithms to compare two myVertex instannces. 
	// std::priority_queue<myVertex*, vector<myVertex*>, myVertex> returns the points in decreasing order of their angles
	// minHeap<myVertex*, myVertex> returns the points in increasing order of their angles
	bool operator() (const myVertex* a, const myVertex* b) const
	{
		return (a->angle < b->angle);
	}
};


struct triangleT
{
	int a,b,c;
};

class myPolygon
{
public:
	myPolygon() : head(NULL), nbVertices(0), v(NULL), hasAngles(false), hasEdgeLengths(false), triangles(nbVertices+2) {}
	~myPolygon() {}
//	void addVertex(outgoingEdgeLen);
	bool init(LightVector<MyREAL> angles, LightVector<MyREAL> edgeLength);
	//bool init(LightVector<XY>& points); //in/out parameter, new points will be added to it.
	myPolygon(LightVector<XY>& vertices) : v(&vertices), nbVertices(vertices.size()), hasEdgeLengths(false), hasAngles(false), triangles(nbVertices+2) {}
	void addVertices(LightVector<int>& v);

	void list();
	void breakDown(MyREAL c, bool clockwise);

	void addOneVertex(myVertex* V, int pointID);
	void addTwoVertices(myVertex* V, int pointID1, int pointID2);
	void connectVertices(myVertex* V);
	void triangulate();

	bool empty() {return nbVertices==0;}
	myPolygon& operator=(LightVector<XY>& vertices);
	myPolygon& operator=(pointSet<XY>& ps);
	void computeEdgesAngles();
	
//protected:
	myVertex *head;
	unsigned int nbVertices;
	bool hasEdgeLengths, hasAngles;
	LightVector<triangleT> triangles;

	LightVector<XY>* v; // pointer to points stored outside the polygon
	bool inside(const XY& P); // return true if P is inside the polygon
	bool bounded_side_2(const XY& P); // from CGAL

	// if after=true introduces a new vertex as vertex->next, such as vertex->outgoingEdgeLen{after insert} = p; vertex->next->outgoingEdgeLen{after insert} = vertex->outgoingEdgeLen{before insert} - p;
	// if after=false introduces a new vertex as ...;
	void split(myVertex* vertex, MyREAL p, bool after);

private:
	MyREAL angle(myVertex* aux);
	myVertex* remove(myVertex* aux);
	void addTriangle(int pointNr1,int pointNr2, int pointNr3);
	bool intersectBoundary(myVertex* vertexlist, int start, int end);
	bool intersectEdge(int as, int ae, int bs, int be);// intersection of segment (as, ae) with (bs, be). A common end point does not count as intersection.
	void clear();


	//bool createOneTriangle(tripleIterator<VERTEX>& ti);
	QuadTree<XY> quad;
};


#endif