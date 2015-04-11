#include "heap.h"
#include "point.h"
#include "polygon.h"
#include "quadtree.h"
#include "vector.h"
#include <iostream>
#include <fstream>
#include <queue>
//#include <boost/heap/priority_queue.hpp>
#include <math.h>

using namespace std;


const MyREAL ASPECTRATIO = 1.5;
const MyREAL SPLITRATIO = ASPECTRATIO * 0.95f;


template<typename T>
inline T sq(T x) {return x*x;}

template<typename T, typename S>
inline T min(T x, S y) {return x<y?x:y;}


//http://stackoverflow.com/questions/15311018/c-procedure-for-determining-whether-two-segments-intersect


double CCW(XY& a, XY& b, XY& c)
{
	return (b.x()-a.x())*(c.y()-a.y()) - (b.y()-a.y())*(c.x()-a.x());
}


bool middle(double a, double b, double c)
{
	double t;    
	if ( a > b )
	{
		t = a;
		a = b;
		b = t;
	}
	if ( a <= c && c <= b ) return true;
	return false;
}

bool intersects(XY& as, XY& ae, XY& bs, XY& be)
{
	if ( ( CCW(as, ae, bs) * CCW(as, ae, be) < 0 ) && ( CCW(bs, be, as) * CCW(bs, be, ae) < 0 ) ) return true;

	if ( CCW(as, ae, bs) == 0.0 && middle(as.x(), ae.x(), bs.x()) && middle(as.y(), ae.y(), bs.y()) ) return true;
	if ( CCW(as, ae, be) == 0.0 && middle(as.x(), ae.x(), be.x()) && middle(as.y(), ae.y(), be.y()) ) return true;
	if ( CCW(bs, be, as) == 0.0 && middle(bs.x(), be.x(), as.x()) && middle(bs.y(), be.y(), as.y()) ) return true;
	if ( CCW(bs, be, ae) == 0.0 && middle(bs.x(), be.x(), ae.x()) && middle(bs.y(), be.y(), ae.y()) ) return true;

	return false;
} // bool intersects(segment& a, segment& b)



////////////////////////////////////////////
void myVertex::clear()
{
//	if ()
	if (this==NULL)
		return;
	myVertex* aux =  this->next;
	myVertex* tmp;
	while (aux!=this)
	{
		tmp = aux;
		aux= aux->next;
		delete tmp;
	}
	delete this;
}

void myPolygon::split(myVertex* vertex, MyREAL p, bool after)
{
	// insert new vertex that splits after->outgoingEdgeLen at p
	myVertex *aux = new myVertex();
	aux->inserted = true;
	aux->angle = 180;
	MyREAL len = vertex->outgoingEdgeLen;
	vertex->outgoingEdgeLen = p;
	aux->outgoingEdgeLen = len - p;

	LightVector<XY>& vptr = *v;
	XY P = XYCustomAverage(p/len, vptr[vertex->pointID], vptr[vertex->next->pointID]);
	aux->pointID = vptr.size();
	vptr.push_back(P);
	++nbVertices;

	// does not change the angle in this.
	if (after)
	{ // insert after "this"
		aux->next = vertex->next;
		aux->prev = vertex;
		vertex->next->prev = aux;
		vertex->next = aux;
	} else
	{// inser before "this"
		// does not change the angle in after.
		aux->next = vertex;
		aux->prev = vertex->prev;
		vertex->prev->next = aux;
		vertex->prev = aux;
	}
} //void myPolygon::split(myVertex* after, double ratio)


void myPolygon::addOneVertex(myVertex* V, int pointID)
{
// before adding V1->this->V2, after adding V1->newPoint->V2 and this is removed from boundary
// adds one point after "this" and replace the outgoing edge with two new edges through the new point
// resulting triangle is added to triangle list
}

void myPolygon::addTwoVertices(myVertex* V, int pointID1, int pointID2)
{
// adds two point after "this" and replace the outgoing edge with three new edges through the new point1 and point2
// resulting two triangle is added to triangle list
}

void myPolygon::connectVertices(myVertex* V)
{
// adds a new edge pointing to this->next->next and removes this->next
}



bool myPolygon::init(LightVector<MyREAL> angles, LightVector<MyREAL> edgeLengths)
{
	// angles are in grades not radians
	unsigned int i;
	nbVertices = angles.size();
	if (nbVertices != edgeLengths.size()) return false;
	if (nbVertices<3) return false;

//	head = aux;
	head = new myVertex(); 
	head->angle = angles[0];
	head->inserted = false;
	head->outgoingEdgeLen = edgeLengths[0];

	myVertex *aux = head; 

	for (i=1; i<nbVertices; ++i)
	{
		aux->next = new myVertex(); 
		aux->next->prev = aux;
		aux = aux->next;
		aux->angle = angles[i];
		aux->outgoingEdgeLen= edgeLengths[i];
		aux->inserted = false;
	}
	aux->next = head;
	head->prev = aux;
	return true;
} // bool myPolygon::init(LightVector<double> angles, LightVector<double> edgeLengths)


void myPolygon::list()
{
	myVertex* aux = head;
	for (unsigned i=0; i<nbVertices-1 ; ++i)
	{
		cout<<aux->outgoingEdgeLen;
		if (aux->inserted) cout<< "+";
		cout<<" -> ";
		aux = aux->next;
	}
	cout<<aux->outgoingEdgeLen<<endl;
}


void myPolygon::breakDown(MyREAL c, bool clockwise)
{
	// introduces intermediary points to the contour until that the ratio of any two consecutive edges is below c;
	myVertex* aux;
	bool split = false;
	MyREAL p, plo, phi, rootlo, roothi;
	MyREAL lm, len, lp, tmp;

	aux = head;
	do
	{
		// this edge is much longer than previous one
		if (clockwise)
		{
			lm = aux->prev->outgoingEdgeLen;
			lp = aux->next->outgoingEdgeLen;
		} else
		{
			lp = aux->prev->outgoingEdgeLen;
			lm = aux->next->outgoingEdgeLen;		
		}
		len = aux->outgoingEdgeLen;
		if (len > c * lm)
		{
			rootlo = plo = lm / c;
			roothi = phi = lm * c;
			// condition a
			if (len < c*(c+1)*lm)
			{
				tmp = len/(c+1);
				if (tmp>rootlo) rootlo = tmp;
				// condition b never fails if c>1.34
				// condition c
				tmp = len-lp/c;
				//if (tmp>lm/c)
				if (tmp>rootlo)
				{
					if (tmp<roothi) roothi = tmp;
					// condition d
					tmp = len-lp * c;
					if (tmp>rootlo)
					//if (tmp>lm/c)
					{
						if (tmp<roothi) roothi = tmp;
						p = (rootlo + roothi) / 2.0f;
					} else p = rootlo*1.01f;
				} else p = rootlo*1.01f;
			} else p = phi*0.99f; // if condition a is not satisfied
			// this is a security check in case the multiplications by .99 or 1.01 bring the value of p outside the interval.
			if (p>phi || p<plo) p = (plo + phi) / 2.0f;
			//insert(aux, p);
			this->split(aux, p, clockwise);
			// XXX TODO create a point if a vertex has been split
			++nbVertices;
			split = true;
		}
		if (clockwise)
		{
			aux = aux->next;
		} else
		{
			aux = aux->prev;
		}
	} while (aux!=head);
} // void myPolygon::breakDown(double c)


myPolygon& myPolygon::operator=(LightVector<XY>& vertices)
{
	v = &vertices;
	hasEdgeLengths = false;
	hasAngles = false;
	head->clear(); head = NULL;
	quad.Clear();
	MyREAL minX, minY, maxX, maxY;
	nbVertices = vertices.size();
	if (nbVertices==0) return *this;
	
	minX = maxX = vertices[0].x();
	minY = maxY = vertices[0].y();

	myVertex * aux;

	for (unsigned int i=0; i<nbVertices; ++i)
	{
		aux = new myVertex();
		aux->inserted = false;
		aux->pointID = i;
		if (head==NULL) 
		{
			head = aux;
			head->next = aux;
			head->prev =  aux;
		} else
		{
			aux->next = head;
			aux->prev = head->prev;
			head->prev = aux;
			aux->prev->next = aux;
		}
		//	pointlist.push_back(i);
		if (vertices[i].x() < minX) minX = vertices[i].x();
		if (vertices[i].y() < minY) minY = vertices[i].y();
		if (vertices[i].x() > maxX) maxX = vertices[i].x();
		if (vertices[i].y() > maxY) maxY = vertices[i].y();
	}
	quad.Resize(Rect<XY>(minX, minY, maxX, maxY));
	for (unsigned int i=0; i<nbVertices; ++i)
	{
		quad.Insert(vertices[i]);
	}
	return *this;
} // myPolygon& polygon::operator=(LightVector<XY>& vertices)



myPolygon& myPolygon::operator=(pointSet<XY>& ps)
{
	return operator=(ps.getPointList());
} // myPolygon& polygon::operator=(pointSet& ps)


MyREAL myPolygon::angle(myVertex* aux)
{
	MyREAL theta;
	LightVector<XY>& vptr = *v;
	theta = XYangle(vptr[aux->prev->pointID]-vptr[aux->pointID], vptr[aux->next->pointID]-vptr[aux->pointID]);
	return theta;
} // MyREAL angle(XY qp, XY qr)


void myPolygon::computeEdgesAngles()
{
	if (head==NULL) return;

	myVertex* aux = head;
	myVertex* tmp;
	LightVector<XY>& vptr = *v;
	bool anticlockwise;

	MyREAL sumAngles = 0;

	do
	{
		aux->outgoingEdgeLen = XYdistance(vptr[aux->pointID], vptr[aux->next->pointID]);
		
		aux->angle = angle(aux);
		sumAngles += aux->angle;
//		cout<<aux->angle <<endl;
		aux = aux->next;
	} while (aux != head);
//	cout<<"------"<<endl;
	anticlockwise = sumAngles > (180.1*(nbVertices-2)); // using 180.1 to account for rounding errors
	if (anticlockwise)
	{
		// reverse the list
		aux = head;
		sumAngles = 0;
		do
		{
			tmp = aux->next;
			aux->next = aux->prev;
			aux->prev = tmp;
			// recompute the angle
			aux->angle = XYangle(vptr[aux->prev->pointID]-vptr[aux->pointID], vptr[aux->next->pointID]-vptr[aux->pointID]);
			sumAngles += aux->angle;
//			cout<<aux->angle <<endl;
			aux = aux->prev;
		} while (aux != head);	
	}

	hasAngles = true;
} // void myPolygon::computeEdgesAngles()


myVertex* myPolygon::remove(myVertex* aux)
{
	myVertex* tmp = aux->prev;
	LightVector<XY>& vptr = *v;

	tmp->next = aux->next;
	aux->next->prev = tmp;
	tmp->angle = angle(tmp);
	tmp->next->angle = angle(tmp->next);
	tmp->outgoingEdgeLen = XYdistance(vptr[tmp->pointID], vptr[tmp->next->pointID]);
	//delete aux;
//	delnodes.push_back(aux);
	--nbVertices;
	return tmp;
} // myPolygon::remove(myVertex* aux)


void myPolygon::addTriangle(int pointNr1,int pointNr2, int pointNr3)
{
	triangleT t;
	t.a = pointNr1;
	t.b = pointNr2;
	t.c = pointNr3;

	triangles.push_back(t);
} // void myPolygon::addTriangle(int pointNr1,int pointNr2, int pointNr3)


bool myPolygon::intersectEdge(int as, int ae, int bs, int be)
{
	LightVector<XY>& vptr = *v;
	if (as==bs || as==be) return false;
	if (ae==bs || ae==be) return false;
	return intersects(vptr[as], vptr[ae], vptr[bs], vptr[be]);
} // bool myPolygon::intersect(int as, int ae, int bs, int be)


// returns false if the segment does not cross any boundary edges and it entirely inside the polygon
bool myPolygon::intersectBoundary(myVertex* vertexlist, int start, int end)
{
	// check if segment crosses any edge
	myVertex* aux = vertexlist;
	bool inte;
	do
	{	
		inte = intersectEdge(aux->pointID, aux->next->pointID, start, end);
		//inte = intersects(vptr[aux->pointID], vptr[aux->next->pointID], vptr[start], vptr[end]);
		if (inte) 
			return true;
		aux = aux->prev;
	} while (aux != vertexlist);
	// if we reach this point the segment does not intersect the boundary edges
	// now check if segment is entirely outside the polygon by looking at its mid point
	LightVector<XY>& vptr = *v;
	XY mid = XYaverage(vptr[start], vptr[end]);
	return ! inside(mid);
} // bool myPolygon::intersectBoundary(int start, int end)


void myPolygon::clear()
{
	head->clear();
	delete head;
	head = NULL;
	nbVertices=0;
} // void myPolygon::clear()


void myPolygon::triangulate()
{
//	std::priority_queue<myVertex*, vector<myVertex*>, myVertex> pq;
//	boost::heap::priority_queue<myVertex*, boost::heap::compare<myVertex> > pq;
	std::priority_queue<myVertex*, vector<myVertex*>, myVertex> fbq;// fallback queue for points that could not be processed during first step


	minHeap<myVertex*> pq;
	LightVector<XY>& vptr = *v;
	XY P, Q;
	MyREAL r;
	LightVector<XY> neighbours;

	if (nbVertices<3)
	{
		throw ("Polygon has less than 3 points.");
	}

	if (!hasAngles) 
	{
		computeEdgesAngles();
	}
	//preprocessing
	myVertex* aux =  head;
	do
	{	
		pq.push(aux);
		cout<<aux->pointID<<"   "<<vptr[aux->pointID]<<endl;
		aux = aux->next;
	} while (aux != head);	

	pq.makeHeap();
// Check ordering
/*	while (!pq.empty())
	{
		aux = pq.popTop();
		cout<<aux->angle<<"  "<<vptr[aux->pointID]<<endl;
	//	remove(aux);
	}*/

	while (nbVertices>0)
	{
		aux = pq.popTop();
		if (aux==NULL) throw "This should never happen: internal data inconsistency.";
		if (nbVertices==3)
		{
			addTriangle(aux->prev->pointID, aux->pointID, aux->next->pointID);
			clear();
		}
		if (nbVertices==4)
		{
			if (XYdistance(vptr[aux->pointID], vptr[aux->next->next->pointID]) < XYdistance(vptr[aux->prev->pointID], vptr[aux->next->pointID]))
			{
				addTriangle(aux->pointID, aux->next->pointID, aux->next->next->pointID);
				addTriangle(aux->prev->prev->pointID, aux->prev->pointID, aux->pointID);	
			} else
			{
				addTriangle(aux->pointID, aux->next->pointID, aux->prev->pointID);
				addTriangle(aux->next->pointID, aux->next->next->pointID, aux->prev->pointID);
			}
			clear();
		}

		// small angles below 75°
		if (aux->angle<=75)
		{
			if (!intersectBoundary(aux, aux->next->pointID, aux->prev->pointID))
			{
				addTriangle(aux->prev->pointID, aux->pointID, aux->next->pointID);
				remove(aux);
				pq.makeHeap();
			} else
			{
				fbq.push(aux);
			}
			continue;
		}
		// medium angles 75°..135°
		if (aux->angle>75 && aux->angle<=135)
		{
			nbVertices = 0; // force exit

			// compute candidate point
			P = vptr[aux->next->pointID] + vptr[aux->prev->pointID] - vptr[aux->pointID];
			// check if another boundary point is close to P
			r = XYdistance(P,XYaverage(vptr[aux->prev->pointID], vptr[aux->next->pointID]));
			neighbours = quad.QueryRadius(P, r*0.5f);
			if (neighbours.size()>0)
			{

			}
			if (neighbours.size()==0)
			{
			// check that lines are not intersecting triangle
			// then create 2 triangles
			}
			continue;
		}

		// large angles 135°..180°
		if (aux->angle>135 && aux->angle<=180)
		{
			nbVertices = 0; // force exit

			// compute candidate points
			//P = vptr[aux->prev->pointID] + vptr[aux->next->pointID] - vptr[aux->pointID];
			//Q = vptr[aux->prev->pointID] + vptr[aux->next->pointID] - vptr[aux->pointID];
			// check if another boundary point is already 
		}
	} // while (nbVertices>0)
} // void myPolygon::triangulate()



// return true if P is inside the polygon
bool myPolygon::inside(const XY& P) 
{
// uses the ray-tracing algorythm, that count the number of intersections between the edges of the polygon and a ray originating from the point
// if the number of intersections is odd the point is inside. We use a ray aparallel to the x axis.
// http://stackoverflow.com/questions/217578/point-in-polygon-aka-hit-test/2922778#2922778
//	http://alienryderflex.com/polygon/
	bool    oddCrossing = false;    // the  crossing number counter

	myVertex* aux = head;
	LightVector<XY>& vptr = *v;
	XY Q, R, S;
	if (head==NULL) return false;
	do
	{
		Q = vptr[aux->pointID];
		R = vptr[aux->next->pointID];
		S = vptr[aux->next->next->pointID];
	   if (((Q.y() < P.y()) && (R.y() >= P.y())) || ((R.y() < P.y()) && (Q.y() >=  P.y())))
	   { 
            // compute  the actual edge-ray intersect x-coordinate
            MyREAL vt = Q.x() +(MyREAL)(P.y() - Q.y()) / (R.y() - Q.y())*(R.x() - Q.x());
            if (P.x() < vt ) // P.x < intersect
                 oddCrossing = ! oddCrossing;   // a valid crossing of y=P.y right of P.x
        }
		aux = aux->next;
	} while (aux->next != head);

    return oddCrossing;    // 
} // bool myPolygon::inside(const XY& P)

