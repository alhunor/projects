#ifndef _CGALLLL_H
#define _CGALLLL_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Cartesian.h>
#include <iostream>
#include <fstream>

#include "point.h"
#include "quadtree.h"
#include "vector.h"



//http://doc.cgal.org/latest/Kernel_23/index.html

class myPoint : public CGAL::Exact_predicates_inexact_constructions_kernel::Point_2
{
public: 
	myPoint () : CGAL::Exact_predicates_inexact_constructions_kernel::Point_2(0,0), id(-1) {}
	myPoint (double x, double y, int _id=-1) : CGAL::Exact_predicates_inexact_constructions_kernel::Point_2(x,y), id(_id) {}
	int id;
};


// K_ is the new kernel, and K_Base is the old kernel
template < typename K, typename K_Base >
class MyCartesian_base : public K_Base::template Base<K>::Type
{
	typedef typename K_Base::template Base<K>::Type   OldK;
public:
	typedef K		Kernel;
	typedef myPoint	Point_2;
};

template < typename FT_ >
struct MyKernel : public CGAL::Type_equality_wrapper<MyCartesian_base<MyKernel<FT_>, CGAL::Cartesian<FT_> >, MyKernel<FT_> > {};


typedef CGAL::Filtered_kernel_adaptor<MyKernel<double> > K;
typedef K::Point_2 Point;
typedef CGAL::Polygon_2<K> Polygon_2;

template <class POINT>
class cgal
{
public:
	cgal() : hasEdgeLengths(false), hasAngles(false) {}

	cgal& operator=(pointSet<POINT>& ps);
	cgal& operator=(LightVector<POINT>& vertices);

	void polyStats();
	bool inside(const POINT& p) { return poly.has_on_bounded_side(p); }
private:
	Polygon_2 poly;
	QuadTree<POINT> quad;
//	LightVector<triangleT> triangles;
	LightVector<POINT>* v; // pointer to points stored outside the polygon
	bool hasEdgeLengths, hasAngles;
	unsigned int nbVertices;
};

template <class POINT>
void cgal<POINT>::polyStats()
{
	// check if the polygon is simple
	std::cout << "The polygon is " <<(poly.is_simple() ? "" : "not ") << "simple." << std::endl;
	// check if the polygon is convex
	std::cout << "The polygon is " <<(poly.is_convex() ? "" : "not ") << "convex." << std::endl;
	std::cout << "The polygon has " << poly.size() << " vertices." << std::endl;
  
	Polygon_2::Vertex_const_circulator vc, front;
	vc = front = poly.vertices_circulator();
	do
	{
		Point p = *vc;
		std::cout<<vc->id<<"   "<<vc->x()<<"  "<<vc->y()<<std::endl;
		++vc;
	} while (vc !=front);
}

/*
bool cgal::loadPolyFromFile(char* filename)
{
	unsigned int nr;
	std::ifstream in;
	in.open(filename);	
	if (in.bad()) return false;
	in>>nr;
	std::vector<Point> points(nr);
	MyREAL x,y;
	int id;
	for(unsigned int i=0; i<nr; ++i)
	{
		in>> id >> x >> y;
		points[i] = Point(x,y, id);
	}
	in.close();
	poly= Polygon_2(points.begin(), points.end());
	return true;
} // bool pointSet::load(char* filenamme)
*/

template <class POINT>
cgal<POINT>& cgal<POINT>::operator=(LightVector<POINT>& vertices)
{
	v = &vertices;
	hasEdgeLengths = false;
	hasAngles = false;
	quad.Clear();
	MyREAL minX, minY, maxX, maxY;
	nbVertices = vertices.size();
	if (nbVertices==0) return *this;
	
	minX = maxX = vertices[0].x();
	minY = maxY = vertices[0].y();

	for (unsigned int i=1; i<nbVertices; ++i)
	{
		if (vertices[i].x() < minX) minX = vertices[i].x();
		if (vertices[i].y() < minY) minY = vertices[i].y();
		if (vertices[i].x() > maxX) maxX = vertices[i].x();
		if (vertices[i].y() > maxY) maxY = vertices[i].y();
		//	pointlist.push_back(i);
	}
	quad.Resize(Rect<POINT>(minX, minY, maxX, maxY));
	for (unsigned int i=0; i<nbVertices; ++i)
	{
		//TODO convert to XY
//		quad.Insert(vertices[i]);
	}
	return *this;
} // cgal& cgal::operator=(LightVector<myPoint>& vertices)


template <class POINT>
cgal<POINT>& cgal<POINT>::operator=(pointSet<POINT>& ps)
{
	return operator=(ps.getPointList());
} // myPolygon& polygon::operator=(pointSet& ps)


#endif