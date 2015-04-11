#include "point.h"

MyREAL XYdistance(XY p, XY q)
{
	MyREAL d = (p-q).norm();
	return d;
}

// XY is a small object, I believe that passing by value is not much slower than passing by reference
// TODO test above hypothesis
MyREAL XYcross(XY qp, XY qr)
//float myPolygon::cross(int p, int q, int r)
{
	//XY qp, qr;
	//qp = v[p]-v[q];
	//qr = v[r]-v[q];
	MyREAL d = qp.x()*qr.y() - qp.y()*qr.x(); 
	return d;
}

MyREAL XYdot(XY qp, XY qr)
//float myPolygon::dot(int p, int q, int r)
{
	//XY qp, qr;
	//qp = v[p]-v[q];
	//qr = v[r]-v[q];
	MyREAL d = qp.x()*qr.x() + qp.y()*qr.y(); 
	return d;
}

MyREAL XYangle(XY qp, XY qr)
{
	MyREAL cosTheta, sinTheta, theta, invnorm;

	invnorm = 1 / (qp.norm()*qr.norm());
//	return theta;

	sinTheta = XYcross(qp, qr) * invnorm;
	cosTheta = XYdot(qp, qr) * invnorm ;
	theta = acos(cosTheta) * PITOGRAD;
	if (sinTheta<0)
	{
		theta = 360-theta;
	}
	return theta;
} // float angle(XY qp, XY qr)


XY XYaverage(XY p, XY q)
{
	XY mid((p.x()+q.x())/2, (p.y()+q.y())/2);
	return mid;
} // XY XYaverage(XY p, XY q)

XY XYCustomAverage(MyREAL r, XY p, XY q)
{
	MyREAL r2 = 1-r;
	XY mid(p.x()*r+q.x()*r2, p.y()*r+q.y()*r2);
	return mid;
}


std::ostream& operator<<(std::ostream& os, const XY p)
{
	std::cout<< "X="<<p.x()<<"  Y="<<p.y();
	return os;
}

