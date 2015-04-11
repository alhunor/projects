#ifndef QUADTREE_H_
#define QUADTREE_H_

#include "vector.h"
#include "point.h" // XY

template <class POINT>
struct Rect
{
	MyREAL x, y, x2, y2; // (x,y) is top left corner, (x2,y2) is bottom right corner
	Rect() : x(0), y(0), x2(0), y2(0) { }
	Rect(MyREAL X, MyREAL Y, MyREAL X2, MyREAL Y2) : x(X), y(Y), x2(X2), y2(Y2) { }
	Rect(const POINT& A, const POINT& B) : x(A.x()), y(A.y()), x2(B.x()), y2(B.y()) { }

	inline bool Contains(const POINT& p) const { return (p.x() >= x && p.y() >= y && p.x() < x2 && p.y() < x2); } 
	inline bool Intersects(const Rect<POINT>& r) const { return !(r.x > x2 || r.x2 < x || r.y > y2 || r.y2 < y); }
	bool Intersects(const POINT& centre, MyREAL radius2) const;
};

//A single layer of a quad tree
template <class POINT>
class QuadTree
{
public:
	// Arbitrary constant to indicate how many elements can be stored in this quad tree node
	static const int QT_NODE_CAPACITY = 4;

	// Axis-aligned bounding box representing the boundaries of this quad tree
	Rect<POINT> bounds;

	//data inside
	POINT points[4];
	int sz; // number of points in XY;

	QuadTree* ne;
	QuadTree* se;
	QuadTree* nw;
	QuadTree* sw;

	//Create a new quadtree
	QuadTree(Rect<POINT> _Bounds) : ne(NULL), se (NULL), nw(NULL), sw (NULL),	bounds(_Bounds), sz(0) {}
	QuadTree() : ne(NULL), se (NULL), nw(NULL), sw (NULL) {} // default constructor of Rect bounds sets up an empry rectangle around 0.
	
	bool Insert(const POINT& p);
	bool Subdivide();
	void Clear();
	void Resize(const Rect<POINT>& NewBounds);
	bool Empty() {return sz == 0;}

	LightVector<POINT> QueryRange(const Rect<POINT>& range) const;
	void QueryRange(const Rect<POINT>& range, LightVector<POINT>& pointsInRange) const;

	LightVector<POINT> QueryRadius(const POINT& centre, MyREAL radius);
	void QueryRadius(const Rect<POINT>& range, LightVector<POINT>& pointsInRange, const POINT& centre, MyREAL radius);
//	closestNeighbour(const XY& centre);
};


template <class POINT>
bool Rect<POINT>::Intersects(const POINT& centre, MyREAL radius2) const
{
	// (x,y) is top left corner, (x2,y2) is bottom right corner
	if ((centre-POINT(x,y)).normsq()<radius2) return true;
	if ((centre-POINT(x,y2)).normsq()<radius2) return true;
	if ((centre-POINT(x2,y)).normsq()<radius2) return true;
	if ((centre-POINT(x2,y2)).normsq()<radius2) return true;
	return false;
}

template <class POINT>
bool QuadTree<POINT>::Insert(const POINT& p)
{
	//Ignore objects which are outside
	if (!bounds.Contains(p))
		return false;

	//If there is space in this quad tree, add the object here
	if (sz < QT_NODE_CAPACITY)
	{
		points[sz++] = p;
		return true;
	}

	//Otherwise, we need to subdivide then add the point to whichever node will accept it
	if (nw == NULL)
		Subdivide();

	if (nw->Insert(p) || ne->Insert(p) || sw->Insert(p) || se->Insert(p))
		return true;

	//Otherwise, the point cannot be inserted for some unknown reason (which should never happen)
	return false;
}

template <class POINT>
bool QuadTree<POINT>::Subdivide()
{
	if (ne != NULL || nw != NULL || sw != NULL || se != NULL)
		return false;

	MyREAL midx = (bounds.x + bounds.x2) /2;
	MyREAL midy = (bounds.y + bounds.y2) /2;

	ne = new QuadTree(Rect<POINT>(midx, bounds.y, bounds.x2, midy));
	se = new QuadTree(Rect<POINT>(midx, midy, bounds.x2, bounds.y2));
	nw = new QuadTree(Rect<POINT>(bounds.x, bounds.y, midx, midy));
	sw = new QuadTree(Rect<POINT>(bounds.x, midy, midx, bounds.y2));
	return true;
}

template <class POINT>
void QuadTree<POINT>::Clear()
{
	if (nw != NULL)
	{
		nw->Clear();
		delete nw;
		nw = NULL;
	}
	if (ne != NULL)
	{
		ne->Clear();
		delete ne;
		ne = NULL;
	}
	if (sw != NULL)
	{
		sw->Clear();
		delete sw;
		sw = NULL;
	}
	if (se != NULL)
	{
		se->Clear();
		delete se;
		se = NULL;
	}
	sz = 0;
} // void QuadTree::DelChildren()

template <class POINT>
void QuadTree<POINT>::Resize(const Rect<POINT>& New)
{
	bounds = New;
	MyREAL midx = (bounds.x + bounds.x2) /2;
	MyREAL midy = (bounds.y + bounds.y2) /2;

	if (ne != NULL) ne->Resize(Rect<POINT>(midx, bounds.y, bounds.x2, midy));
	if (se != NULL) nw->Resize(Rect<POINT>(midx, midy, bounds.x2, bounds.y2));
	if (nw != NULL) se->Resize(Rect<POINT>(bounds.x, bounds.y, midx, midy));
	if (sw != NULL) sw->Resize(Rect<POINT>(bounds.x, midy, midx, bounds.y2));
}

template <class POINT>
LightVector<POINT> QuadTree<POINT>::QueryRange(const Rect<POINT>& range) const
{
	// Prepare an array of results
	LightVector<POINT> pointsInRange;

	QueryRange(range, pointsInRange);

	return pointsInRange;
} // LightVector<XY> QuadTree::QueryRange(Rect& range)

template <class POINT>
void QuadTree<POINT>::QueryRange(const Rect<POINT>& range, LightVector<POINT>& pointsInRange) const
{

	// Automatically abort if the range does not collide with this quad
	if (!bounds.Intersects(range))
		return; // empty list

	// Check objects at this quad level
	for (int p = 0; p < sz; ++p)
	{
		if (range.Contains(points[p]))
			pointsInRange.push_back(points[p]);
	}

	// Terminate here, if there are no children
	if (nw == NULL)
		return;

	// Otherwise, add the points from the children
	ne->QueryRange(range, pointsInRange);
	se->QueryRange(range, pointsInRange);
	nw->QueryRange(range, pointsInRange);
	sw->QueryRange(range, pointsInRange);
} // LightVector<XY> QuadTree::QueryRange(Rect& range)


template <class POINT>
LightVector<POINT> QuadTree<POINT>::QueryRadius(const POINT& centre, MyREAL radius)
{
	// Prepare an array of results
	LightVector<POINT> pointsInRange;
	Rect<POINT> range(centre.x()-radius, centre.y()-radius, centre.x()+radius, centre.y()+radius);

	QueryRadius(range, pointsInRange, centre, radius*radius);

	return pointsInRange;
}

template <class POINT>
void QuadTree<POINT>::QueryRadius(const Rect<POINT>& range, LightVector<POINT>& pointsInRange, const POINT& centre, MyREAL radius2)
{
	// Automatically abort if the range does not collide with this quad
	if (!bounds.Intersects(centre, radius2))
		return; // empty list

	// Check objects at this quad level
	for (int p = 0; p < sz; ++p)
	{
		if ((points[p]-centre).normsq() <= radius2)
			pointsInRange.push_back(points[p]);
	}

	// Terminate here, if there are no children
	if (nw == NULL)
		return;

	// Otherwise, add the points from the children
	ne->QueryRadius(range, pointsInRange, centre, radius2);
	se->QueryRadius(range, pointsInRange, centre, radius2);
	nw->QueryRadius(range, pointsInRange, centre, radius2);
	sw->QueryRadius(range, pointsInRange, centre, radius2);
}


#endif