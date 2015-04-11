#ifndef _HOLEFILLER_H
#define _HOLEFILLER_H

#pragma  warning (disable:4244) // loss of precision when converting double to float
#pragma  warning (disable:4305) // truncation from 'double' to 'const float'

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#define PRECISION double

//#include <levmar.h>
#include <cmath>
#include <list>

//#include "polygon.h"
#include "meshUtil.h"
#include "triangle.h"
#include "vector.h"


//typedef OpenMesh::PolyMesh_ArrayKernelT<>  PolyMesh;
typedef LightVector<TriMesh::HalfedgeHandle> hehV;

template<typename T>
inline T sq(T x) {return x*x;}

template<typename T>
inline T min3(T a, T b, T c)
{
	if (a<b) return min(a,c); else return min(b,c);
}



struct attributeType
{
	double dim3;
	OpenMesh::VertexHandle vhandle;
	OpenMesh::Vec3f normal;
	OpenMesh::Vec3f desirednormal;
};

class holeFiller
{
public:
	holeFiller(TriMesh& _mesh) : mesh(_mesh) {}
	double quality(double * x);
	void computeDim(hehV& hole);
	void fill(int h); // filles holes[h]
	void findHoles();
	void displayHoles();
	void printOff(hehV& hole);
	
protected:
	TriMesh& mesh;
	int dim1, dim2, dim3;

	OpenMesh::Vec3f direction;
	LightVector<TriMesh::FaceHandle> newFaces;
	LightVector<attributeType> newPoints;
	LightVector<hehV> holes;

	int boundaryPoints;
	friend void qual(double *p, double *x, int m, int n, void *data);
};

#endif;