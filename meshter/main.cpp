#pragma  warning (disable:4244) // loss of precision when converting double to float
#pragma  warning (disable:4305) // truncation from 'double' to 'const float'
#pragma  warning (disable:4996) // 


/*
OpenMesh::Vec3f x,y,n,crossproductXY;
...
l = (x-y).length();
n = x.normalize();
scalarProductXY = (x | y);
crossProductXY = x % y;
*/


#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <iostream>
#include <fstream>
#include <levmar.h>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>
#include <OpenMesh/Core/Mesh/Status.hh>
#include <math.h>
const double M_1_SQRT2PI = 0.398942280401433; // 1/sqrt(2pi)
#include "point.h"
#include "holeFiller.h"
#include "polygon.h"
#include "heap.h"
//#include "cgal.h"
//#include "triangle.h"
//#include "vector.h"
//#include "optimizer.h"

#include <Imath/ImathMatrix.h>
#define IM IMATH_INTERNAL_NAMESPACE
typedef IM::Matrix33<PRECISION> Mat3;


/* _USE_MATH_DEFINES are as below
M_E			e			2.718281828
M_LOG2E		log2(e)		1.442695041
M_LOG10E	log10(e)	0.434294482
M_LN2		ln(2)		0.693147181
M_LN10		ln(10)		2.302585093
M_PI		pi			3.141592654
M_PI_2		pi/2		1.570796327
M_PI_4		pi/4		0.785398163
M_1_PI		1/pi		0.318309886
M_2_PI		2/pi		0.636619772
M_2_SQRTPI	2/sqrt(pi)	1.128379167
M_SQRT2		sqrt(2)		1.414213562
M_SQRT1_2	1/sqrt(2)	0.707106781
*/


// ----------------------------------------------------------------------------

using namespace std;
//typedef OpenMesh::TriMesh_ArrayKernelT<MyTraits> TriMesh;
//typedef OpenMesh::PolyMesh_ArrayKernelT<>  PolyMesh;

Mat3 pointsToMat(OpenMesh::Vec3f& A, OpenMesh::Vec3f& B, OpenMesh::Vec3f& C)
{
	Mat3 m;
	m[0][0] = A[0]; 	m[0][1] = A[1]; 	m[0][2] = A[2];
	m[1][0] = B[0]; 	m[1][1] = B[1]; 	m[1][2] = B[2];
	m[2][0] = C[0]; 	m[2][1] = C[1]; 	m[2][2] = C[2];
	return m;
}


bool readmesh(TriMesh&mesh, char* filename)
{
	return OpenMesh::IO::read_mesh(mesh, filename) ;
}


double perimeter(OpenMesh::Vec3f& pointA, OpenMesh::Vec3f& pointB, OpenMesh::Vec3f& pointC)
{
	double d;
	d = (pointB-pointA).length();
	d += (pointC-pointB).length();
	d += (pointA-pointC).length();
	return d;
}

double area(OpenMesh::Vec3f& pointA, OpenMesh::Vec3f& pointB, OpenMesh::Vec3f& pointC)
{
	double d;
	d = 0.5 * cross(pointB-pointA , pointC-pointA).norm();
	return d;
}

double volume(OpenMesh::Vec3f& pointA, OpenMesh::Vec3f& pointB, OpenMesh::Vec3f& pointC)
{
	// http://www.ditutor.com/vectors/volume_tetrahedron.html
	Mat3 m = pointsToMat(pointA , pointB, pointC);
	// use minus sign or change the order of the points in above function call 
	return -m.determinant()/6.0;
}


void analyzeTriangle(OpenMesh::FaceHandle & _fh, TriMesh& mesh)
{
	OpenMesh::Vec3f pointA , pointB , pointC;
	TriMesh::ConstFaceVertexIter cfvIt;
	cfvIt = mesh.cfv_iter(_fh);
	pointA = mesh.point(cfvIt.handle());
	pointB = mesh.point((++cfvIt).handle());
	pointC = mesh.point((++cfvIt).handle());
//	cout<<perimeter(pointA, pointB, pointC) << "   "<< area(pointA, pointB, pointC)<<endl;
	cout<<volume(pointA, pointB, pointC)<<endl; // volume of the tetrahedron with vertices A, B, C and the origin (0,0,0)
	
	//cout<<pointA<<endl;
}



#include "tutorials.cpp"

double meshVolume(TriMesh& mesh)
{
	mesh.request_face_normals();
	mesh.update_normals();
	double vol = 0;
	double surf = 0;

	//iterate through all faces;
	TriMesh::FaceIter f_it, f_end(mesh.faces_end());
	OpenMesh::Vec3f pointA , pointB , pointC;
	TriMesh::ConstFaceVertexIter cfvIt;

	for (f_it = mesh.faces_begin(); f_it!=f_end; ++f_it)
	{
		cfvIt = mesh.cfv_iter(f_it);
		pointA = mesh.point(cfvIt.handle());
		pointB = mesh.point((++cfvIt).handle());
		pointC = mesh.point((++cfvIt).handle());
		surf += area(pointA, pointB, pointC);
		vol += volume(pointA, pointB, pointC);
	}
	return vol;

} // double meshVolume(TriMesh& mesh)


OpenMesh::FaceHandle faceClosestToPoint(TriMesh& mesh, OpenMesh::Vec3f& P)
{
// return a handle to the face that is closes to a give point.
// distance is measured as sum of Euclidean distances from the 3 vertices to the point.
	vector<MyREAL> dist(mesh.n_vertices());
	unsigned int i = 0;

	TriMesh::VertexIter v_it, v_end(mesh.vertices_end());
	for (v_it = mesh.vertices_begin(); v_it!=v_end; ++v_it)
	{
		dist[v_it->idx()]=(P-mesh.point(v_it)).length();
	}

	TriMesh::FaceIter f_it, f_end(mesh.faces_end());
	TriMesh::ConstFaceVertexIter cfvIt;
	double d, mind=10000000000;
	OpenMesh::FaceHandle mindHandle;
	for (f_it = mesh.faces_begin(); f_it!=f_end; ++f_it)
	{
		cfvIt = mesh.cfv_iter(f_it);
		d = dist[ cfvIt.handle().idx()];
		d += dist[ (++cfvIt).handle().idx()];
		d += dist[ (++cfvIt).handle().idx()];
		if (d<mind)
		{
			mind = d;
			mindHandle = f_it.handle();
		}
	} //for (f_it = mesh.faces_begin(); f_it!=f_end; ++f_it)
	return mindHandle ;
} // OpenMesh::FaceHandle faceClosestToPoint(TriMesh& mesh, OpenMesh::Vec3f& P)







double localDensity(TriMesh& mesh, TriMesh::VertexHandle& vh)
{
	// compute localDensity as average distance between the point and its 1-ring neighbourhood

	int valence = 0;
	double dist = 0;
	TriMesh::Point P, O;
	
	O = mesh.point( vh );
	
	TriMesh::VertexVertexIter    vv_it;
	for (vv_it=mesh.vv_iter( vh ); vv_it; ++vv_it)
	{
		P = mesh.point( vv_it );
		dist += (P-O).norm();
		++valence;
	}
	return dist/valence;
} // double localDensity(TriMesh::VertexHandle& vh)



double angleBetweenPoints(TriMesh& mesh, TriMesh::VertexHandle& Ah, TriMesh::VertexHandle& Oh, TriMesh::VertexHandle& Bh)
{
	// returns the angle formed by the points AOC
	TriMesh::Point A, O, B;
	A = mesh.point( Ah );
	O = mesh.point( Oh );
	B = mesh.point( Bh );
	double a = (A-O) | (B-O);
	return a;
}

bool closeHole(TriMesh& mesh, hehV& hole)
{
//	TriMesh::VertexHandle vhandle[3];
	unsigned int n = hole.size();
	unsigned int i;

	if (n<3) throw "Big ugly error.";

	// only one triangle missing
	if (n==3)
	{
		addFace(mesh, mesh.to_vertex_handle(hole[0]), mesh.to_vertex_handle(hole[1]), mesh.to_vertex_handle(hole[2]));
		//addFace(mesh, mesh.halfedge(hole[0]).vertex_handle_, mesh.halfedge(hole[1]).vertex_handle_, mesh.halfedge(hole[2]).vertex_handle_);
		return true;
	}
// just zip it up - trivial ugly solution
//goto alternative;
	unsigned int lo=1;
	unsigned int hi=n-1;
	while (hi>lo)
	{
		addFace(mesh, mesh.to_vertex_handle(hole[lo]), mesh.to_vertex_handle(hole[hi]), mesh.to_vertex_handle(hole[(hi+1) % n]));
		//addFace(mesh, mesh.halfedge(hole[lo]).vertex_handle_, mesh.halfedge(hole[hi]).vertex_handle_, mesh.halfedge(hole[(hi+1) % n]).vertex_handle_);
		lo++;
	
		if (lo==hi) break;
		addFace(mesh, mesh.to_vertex_handle(hole[lo]), mesh.to_vertex_handle(hole[hi]), mesh.to_vertex_handle(hole[lo-1 ]));
		//addFace(mesh, mesh.halfedge(hole[lo]).vertex_handle_, mesh.halfedge(hole[hi]).vertex_handle_, mesh.halfedge(hole[lo-1 ]).vertex_handle_);
		hi--;
	}

	return true;

//alternative:

// alternatively, use advancing front method

	double* tightness = new double [n];
	double* theta = new double [n];
	cout<<"Angles"<<endl;
	TriMesh::Point P;
	for (i=0; i<n; ++i)
	{
		//tightness[i] = localDensity(mesh, mesh.halfedge(hole[i]).vertex_handle_);
		tightness[i] = localDensity(mesh, mesh.to_vertex_handle(hole[i]));
		//theta[i] = angleBetweenPoints(mesh, mesh.halfedge(hole[ (n+i-1) % n]).vertex_handle_, mesh.halfedge(hole[i]).vertex_handle_, mesh.halfedge(hole[ (i+1) % n]).vertex_handle_);
		//P = mesh.point(mesh.halfedge(hole[i]).vertex_handle_);
		P = mesh.point(mesh.to_vertex_handle(hole[i]));
		theta[i] = mesh.calc_sector_angle(hole[i]);
		cout<<tightness[i]<<"  "<<theta[i]<<"  "<<P<<endl;
	}
	delete[] tightness;
	delete[] theta;
	return false;
} // bool closeHole(TriMesh& mesh, hehV& hole)

// http://stackoverflow.com/questions/5963954/fast-templated-c-octree-implementation




void outputGNUPlot(LightVector<XY>& points, char* filename)
{
	ofstream of;
	of.open(filename);
	unsigned int n = points.size();
	for (unsigned int i = 0; i<n; ++i)
	{
		of<<points[i].x()<<"    "<<points[i].y()<<"   "<<points[(i+1)%n].x()<<"    "<<points[(i+1)%n].y()<<endl;
	}
	of.close();
}

pointSet<XY> createPointSetExample()
{
	pointSet<XY> ps;
	ps.add(-19.43, 80.02);
	ps.add(-42.02, 94.08);
	ps.add(-68.8, 76.87);
	ps.add(-97.31, 99.57);
	ps.add(-128, 106.17);
	ps.add(-151.17, 97.52);
	ps.add(-153.56, 76.98);
	ps.add(-148.75, 60.04);
	ps.add(-137.95, 43.05);
	ps.add(-107.17, 53.93);
	ps.add(-64.85, 41.39);
	ps.add(-27.59, 08.88);
	ps.add(-14.34, 43.03);
	ps.add(-2.66, 62.39);
	//ps.save("holeinthehead.pts");
//	points.push_back(XY(-19.43, 80.02, 0)); -- old format
	return ps;
}


void createHexagon(MyREAL r)
{
	pointSet<XY> ps;
	MyREAL x,y;
	for (int i=0; i<6; ++i)
	{
		x = r * cos(i*M_PI/3);
		y = r * sin(i*M_PI/3);
		ps.add(x,y);
	}
	ps.save("hexagon.pts");
}

void createPentagram(MyREAL r)
{
	pointSet<XY> ps;
	MyREAL x,y;
	for (int i=0; i<10; ++i)
	{
		if (i % 2)
		{
			x = r * cos(i*M_PI/5);
			y = r * sin(i*M_PI/5);
		} else
		{
			x = 2 *r * cos(i*M_PI/5);
			y = 2* r * sin(i*M_PI/5);
		}
		ps.add(x,y);
	}
	ps.save("pentagram.pts");
}


// http://www.engr.colostate.edu/~dga/dga/papers/point_in_polygon.pdf
void testMyPolyTriangulation()
{
//	LightVector<XY> points;
	myPolygon poly;

//	pointSet pp("square.pts");
	//poly = pp;
	//cout<<poly.inside(XY(0, 3))<<endl; // inside "square.pts", should be 1
	//cout<<poly.inside(XY(20, -5))<<endl; // // inside "square.pts", should be 0

//	createHexagon(100);
	createPentagram(20);
//	pointSet ps("holeinthehead.pts");
	pointSet<XY> ps("pentagram.pts");


	outputGNUPlot(ps.getPointList(), "hole.dat");
	poly = ps;
	cout<<poly.inside(XY(15,0))<<endl;
	cout<<poly.inside(XY(15,0.5))<<endl;
	cout<<poly.inside(XY(1,1))<<endl;
	cout<<poly.inside(XY(100,100))<<endl;
	cout<<poly.inside(XYaverage((*poly.v)[1],(*poly.v)[9]))<<endl;

	poly.computeEdgesAngles();
//	poly.breakDown(1.618f, true); // use the golden ratio, go clockwise
//	poly.breakDown(1.618f, false); // use the golden ratio, go counter-clockwise
	poly.list();
	poly.triangulate();
	poly.head->clear();
	poly.head = NULL;
} // void testPolyTriangulation()


bool computeNormals(TriMesh mesh)
{
	// this Face property stores the computed centers of gravity
	OpenMesh::FPropHandleT<TriMesh::Point> trueNormals;
	OpenMesh::FPropHandleT<TriMesh::Point> approxNormals;
	mesh.add_property(trueNormals);
	mesh.add_property(approxNormals);
	TriMesh::FaceIter		f_it, f_end(mesh.faces_end());
	double valence;
	//TriMesh::FaceFaceIter	ff_it;

	for (f_it=mesh.faces_begin(); f_it!=f_end; ++f_it)
	{
		mesh.property(trueNormals,f_it).vectorize(0.0f);
		valence = 0;
/*		for (vv_it=mesh.vv_iter( v_it ); vv_it; ++vv_it)
		{
			mesh.property(cogs,v_it) += mesh.point( vv_it );
			++valence;
		}*/
		mesh.property(approxNormals,f_it) /= valence;
	}

}


void main()
{
/*
	cgal<myPoint> cg;
	pointSet<myPoint> ps("pentagram.pts");
	cg = ps;
	cg.polyStats();
	cout<<cg.inside(Point(15,0))<<endl; // yes
	cout<<cg.inside(Point(15,0.5))<<endl; // yes
	cout<<cg.inside(Point(1,1))<<endl; // yes
	cout<<cg.inside(Point(100,100))<<endl; // no
	return;
*/
	
//	testMyPolyTriangulation();
	

/*	LightVector<double> angles(7);
	angles.fill(0);
//	double lens[7] = {3, 6, 8, 10, 4, 9, 2};
	double lens[7] = {10, 3, 8, 10, 4, 9, 7};
	LightVector<double> edgeLengths(7, lens);
	mp.init(angles, edgeLengths);
	mp.list();
	mp.breakDown(1.618, true); // use the golden ratio, go clockwise
	mp.list();
	mp.breakDown(1.618, false); // use the golden ratio, go counter-clockwise
	mp.list();
	mp.head->clear();
	mp.head = NULL;
	return; */

	//tutorial1();
	//tutorial2("tetrahedron.off", "tetrahedron2.off",5);
	//tutorial1Hu();

//	Mat3 m(1,2,3,4,4,6,7,8,9);
//	Mat3 m2 = m.inverse();
//	cout<<m.determinant()<<endl;
//	cout<<m2;

	 TriMesh mesh;
	 //mesh = createSphere(2.0f,4); OpenMesh::IO::write_mesh(mesh, "tetraSphere.off");
	//mesh = createTetra(2); OpenMesh::IO::write_mesh(mesh, "tetra.off"); readmesh(mesh, "tetra.off");

	 TriMesh mesh2 = createTorus(10, 3, 50);
	 OpenMesh::IO::write_mesh(mesh2, "torus.off");


	 return;

//	readmesh(mesh, "sphereholes.off");
	readmesh(mesh, "lyukas.off");

	// suboptimal, add normals to all faces while we need it only for the 1-ring around the hole
	if ( ! mesh.has_face_normals())
		mesh.request_face_normals();
//	mesh.update_normals();

	holeFiller hf(mesh);
	hf.findHoles();
	hf.displayHoles();
//	hf.fill(0);

// Az emberkeben, i.e. readmesh(mesh, "lyukas.off");
//	hf.fill(0); // hat
//	hf.fill(1); // has
	hf.fill(2); // fej
//	printOff(mesh, holes[2]);

	mesh.release_face_normals(); // always release after request

//	closeHole(mesh, holes[1]);
//	closeHole(mesh, holes[0]);
//	closeHole(mesh, holes[2]);


/*	mesh.request_face_normals();
	if (mesh.has_vertex_normals() ) mesh.update_vertex_normals();

  for (; v_it!=v_end; ++v_it)
    this->set_normal(v_it.handle(), calc_vertex_normal(v_it.handle()));
*/



/*	readmesh(mesh, "icosahedron.off");
	OpenMesh::Vec3f P(-2,0.2,2);
	OpenMesh::FaceHandle fh = faceClosestToPoint(mesh,P);
	cout<<fh.idx();
	
	extendMesh(mesh, fh, P, true);
	OpenMesh::IO::write_mesh(mesh, "ico--.off");
	*/

	//cout<<"Volume = "<<meshVolume(mesh)<<endl;
//	OpenMesh::IO::write_mesh(mesh, "spherevolt.off");
	OpenMesh::IO::write_mesh(mesh, "lyukasvolt.off");
} // void main()

