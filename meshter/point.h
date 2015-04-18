#ifndef POINT_H_
#define POINT_H_

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <math.h>
#include <iostream> 
#include "vector.h"

typedef double MyREAL;
const MyREAL PITOGRAD = 180 /(MyREAL)M_PI;

//template<typename T=float>
struct XY
{
	int id; // id of the point, if it comes from a collection. 
	XY(MyREAL X, MyREAL Y, int _id=-1) : x_(X), y_(Y), id(_id) { }
	MyREAL x() const {return x_;}
	MyREAL y() const {return y_;}
	XY() : x_(0), y_(0), id(-1) { }
	XY operator-(const XY& p) const {return XY(x_-p.x_, y_-p.y_); }
	XY operator+(const XY& p) const {return XY(x_+p.x_, y_+p.y_); }
	XY operator/(MyREAL f) const {return XY(x_/f, y_/f); }
	XY operator*(MyREAL f) const {return XY(x_*f, y_*f); }
	// cross product has been removed
//	float operator*(const XY& p) const {return x*p.y-y*p.x; }
	MyREAL norm() {return sqrt(normsq());}
	MyREAL normsq() {return x_*x_+y_*y_;}
protected:
	MyREAL x_, y_;
};


MyREAL XYdistance(XY p, XY q);
MyREAL XYcross(XY qp, XY qr);
MyREAL XYdot(XY qp, XY qr);
MyREAL XYangle(XY qp, XY qr);
XY XYaverage(XY p, XY q);
XY XYCustomAverage(MyREAL r, XY p, XY q);

std::ostream& operator<<(std::ostream& os, const XY p);


template <class POINT>
class pointSet
{
public:
	pointSet() {}
	pointSet(char* filenamme) {load(filenamme);}
	int add(MyREAL x, MyREAL y);
	void save(char* filenamme);
	bool load(char* filenamme);
	LightVector<POINT>& getPointList() {return points;}
protected:
	LightVector<POINT> points;
};



template <class POINT>
int pointSet<POINT>::add(MyREAL x, MyREAL y)
{
	unsigned int nr = points.size();
	POINT P(x, y, nr);
	points.push_back(P);
	return nr;
} // int pointSet::add(double x, double y)

template <class POINT>
void pointSet<POINT>::save(char* filename)
{
	unsigned int nr = points.size();
	ofstream out;
	out.open(filename);
	out<<nr<<endl;
	for(unsigned int i=0; i<nr; ++i)
	{
		out<<points[i].id<< " " << points[i].x()<< " "<<points[i].y()<<endl;
	}
	out.close();
} // void pointSet::save(char* filename)

template <class POINT>
bool pointSet<POINT>::load(char* filename)
{
	unsigned int nr;
	ifstream in;
	in.open(filename);	
	if (in.bad()) return false;
	
	in>>nr;
	points.setSize(nr);
	int id;
	MyREAL x,y;
	for(unsigned int i=0; i<nr; ++i)
	{
		in>>id >> x >> y;
		points[i]=POINT(x,y,id);
	}
	in.close();
	return true;
} // bool pointSet::load(char* filenamme)




#endif