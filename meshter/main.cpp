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

#include <list>
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

#include "ValenceViewer.hh"
#include <ANN/ANN.h>
#include <ANN/ANNperf.h>
#include <ANN/ANNx.h>



class pts
{
public:
	pts() : hasData(false) {};
	~pts() {};
	bool readFile(char* filename);
	int dim;
	int nbPoints;
	bool hasNormals;
	ANNpointArray dataPts;
	ANNpointArray normals;
protected:
	bool hasData;
};

bool pts::readFile(char* filename)
{
	std::ifstream dataIn;

	dataIn.open(filename, std::ios::in);// open data file
	if (!dataIn)
	{
		std::cerr << "Cannot open data file\n";
		return false;
	}
	// read data header for nbPoints, dimension and boolean for has normals or not
	dataIn>>nbPoints>>dim>>hasNormals;

	dataPts = annAllocPts(nbPoints, dim);			// allocate data points
	if (hasNormals)
	{
		normals = annAllocPts(nbPoints, dim);			// allocate storage for normals
	}

	int i,j;

	// reads data
	for (i = 0; i < nbPoints; ++i)
	{
		for ( j = 0; j < dim; ++j)
		{
			dataIn>>dataPts[i][j];
		}
		if (hasNormals)
		{
			for ( j = 0; j < dim; ++j)
			{
				dataIn>>normals[i][j];
			}
		} // if (hasNormals)
	}
	
	hasData = true;
	return true;
}  // bool pts::readFile(char* filename)


void ann_test()
{
	pts pts;
	pts.readFile("sphere.ptsn");

	ANNkd_tree* kdTree = new ANNkd_tree(					// build search structure
					pts.dataPts,					// the data points
					pts.nbPoints,						// number of points
					pts.dim);						// dimension of space

	ANNpoint	queryPt = new ANNcoord[pts.dim];				// query point
	queryPt[0] = 0;
	queryPt[1] = 0;
	queryPt[2] = 0.5;
	int		k		= 3;	// number of nearest neighbors
	double	eps		= 0;	// error bound
	ANNidxArray nnIdx = new ANNidx[k];						// allocate near neigh indices
	ANNdistArray dists = new ANNdist[k];						// allocate near neighbor dists
	bool* marked = new bool[pts.nbPoints];
	
	for (int i = 0; i<pts.nbPoints; ++i) marked[i] = false;


	kdTree->annkSearch(						// search
			queryPt,						// query point
			k,								// number of near neighbors
			nnIdx,							// nearest neighbors (returned)
			dists,							// distance (returned)
			eps, 							// error bound
			marked);

	std::cout << "\tNN:\tIndex\tDistance\n";
	for (int i = 0; i < k; i++)
	{			// print summary
		dists[i] = sqrt(dists[i]);			// unsquare distance
		std::cout << "\t" << i << "\t" << nnIdx[i] << "\t" << dists[i] << "\n";
		marked[nnIdx[i]] = true;
	}

	kdTree->annkSearch(						// search
			queryPt,						// query point
			k,								// number of near neighbors
			nnIdx,							// nearest neighbors (returned)
			dists,							// distance (returned)
			eps, 							// error bound
			marked);

	std::cout << "\tNN:\tIndex\tDistance\n";
	for (int i = 0; i < k; i++)
	{			// print summary
		dists[i] = sqrt(dists[i]);			// unsquare distance
		std::cout << "\t" << i << "\t" << nnIdx[i] << "\t" << dists[i] << "\n";
	}


	delete [] marked;
    delete [] nnIdx;							// clean things up
    delete [] dists;
    delete kdTree;
	annClose();									// done with ANN
}




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


int epoch = 123;

class ring
{
public:	
	ring(TriMesh _mesh) : mesh(_mesh), initialised(false) {}
	~ring() { if (initialised) mesh.remove_property(epochs);}
	bool init();
	LightVector<TriMesh::VertexHandle> getRing(TriMesh::VertexHandle _vh, int k);

private:
	TriMesh mesh;
	bool initialised;
	OpenMesh::VPropHandleT<int> epochs;
};

bool ring::init()
{
	if (!initialised)
	{
		mesh.add_property(epochs);
		initialised =  true;
		++epoch;
		TriMesh::VertexIter v_it, v_end(mesh.vertices_end());
		for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
		{
			mesh.property(epochs,v_it) = epoch;
		}
	}
	// set epochs 

	return true;
}


LightVector<TriMesh::VertexHandle> ring::getRing(TriMesh::VertexHandle _vh, int k)
// returns all vertices that are within the k-ring of the vertex vh
{
	if (!initialised) init();

	int aux;
	int epoch_base = epoch;
	epoch++;
	//std::list<TriMesh::VertexHandle> lik;
	TriMesh::VertexHandle vh;
	TriMesh::VertexVertexIter vvi, vvstart;
	mesh.property(epochs, _vh) = epoch+1;
	LightVector<TriMesh::VertexHandle> vect;
	vect.push_back(_vh);
	unsigned int i = 0;

	while (i < vect.size())
	{
		vh = vect[i];
		aux = mesh.property(epochs, vh);
		if (aux > epoch_base + k+1) break; // we have finished exploring all the vertices from the k-1 ring.
		if (aux == epoch + 1) ++epoch; // finished all vertices in the current epoch, go to next ring

		vvstart = vvi = mesh.vv_iter(vh);
		do
		{
			vh = vvi.handle();
			aux = mesh.property(epochs, vh);
			if (aux <= epoch_base) // node has not been visited yet
			{
				mesh.property(epochs, vh) = epoch+1;
				vect.push_back(vh);
			} else 
			{
				// the node has been visted already and has been added to vect, continue
			}
			++vvi;
		} while (vvi);
		++i; // move to next vertex
	} // i < vect.size()
	return vect;
} // void getRing(TriMesh mesh, TriMesh::VertexHandle _vh, int k)


bool computeNormals(TriMesh mesh)
{

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
	return true;
} // bool computeNormals(TriMesh mesh)

// https://msdn.microsoft.com/en-us/library/ms182372.aspx -< Profiler
int main(int argc, char **argv)
{
	//ann_test();

	glutInit(&argc, argv);

	ValenceViewer window("Wireframe", 512, 512);

//	window.open_mesh("bunny.off");
	window.open_mesh("torus(10,3,50).off");

	glutMainLoop();
	
	
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


	 TriMesh mesh2 = createTorus(10, 3, 50);
	 ring ri(mesh2);
	 LightVector<TriMesh::VertexHandle> vhv = ri.getRing(TriMesh::VertexHandle(0), 2);

	 for (unsigned int i= 0; i<vhv.size(); ++i)
	 {
		cout<< vhv[i]<<"  ";
	 }
	 OpenMesh::IO::write_mesh(mesh2, "torus.off");

	mesh2.request_face_normals();
	mesh2.request_vertex_normals();
	mesh2.update_normals();


	TriMesh::VertexHandle vh = TriMesh::VertexHandle(5);
	TriMesh::VertexFaceIter vfi = mesh2.vf_iter(vh);
	int cc=0;
	OpenMesh::Vec3f norm (0, 0, 0), normv;
	while (vfi)
	{
		cout<<"+"<<vfi.handle()<<endl;
		norm += mesh2.normal(*vfi);
		++cc;
		++vfi;
	}
	double len = norm.length();
	if (len != 0)
	{
		norm *= 1/len;
	}

	normv = mesh2.normal(vh);
	
	meshVolume(mesh2);

	return 0;

	TriMesh mesh;
	 //mesh = createSphere(2.0f,4); OpenMesh::IO::write_mesh(mesh, "tetraSphere.off");
	//mesh = createTetra(2); OpenMesh::IO::write_mesh(mesh, "tetra.off"); readmesh(mesh, "tetra.off");

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


void display(void)
{
	//Clear all pixels
	glClear(GL_COLOR_BUFFER_BIT);

	// draw white polygon (rectangle) with corners at(0.25, 0.25, 0.0) and (0.75, 0.75, 0.0)
	/*glColor3f(1.0,1.0,1.0);
	glBegin(GL_POLYGON);
		glVertex3f(0.25, 0.25, 0.0);
		glVertex3f(0.75, 0.25, 0.0);
		glVertex3f(0.75, 0.75, 0.0);
		glVertex3f(0.25, 0.75, 0.0);
	glEnd();*/
	glColor3f(1.0,0,1.0);
	glutWireCube(1);
    
	// All drawing is stored in a buffer awaiting additional OpenGL commands, until the user call glFlush()
	glFlush();
}



void processmenu( int i)
{
	std::cout<<i<<std::endl;
}


void initOpenGL(void)
{
    //select clearing (background) color
    glClearColor(0.0, 0.0, 0.0, 0.0);

    //initialize viewing values 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
	gluLookAt(0,0,0, 5,4,3, 0,1,0);
    glMatrixMode(GL_MODELVIEW);
}


/*
int main(int argc, char **argv)
{

	ann_test();
	glutInit(&argc, argv);


	//Set Display Mode to single buffered RGB
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	//Set the window size
	glutInitWindowSize(250,250);

	//Set the window position
	glutInitWindowPosition(100,100);

	//Create the window
	glutCreateWindow("My first Window");

	glutDisplayFunc(display);


	int menuID = glutCreateMenu(processmenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutAddMenuEntry("Menu 1", 0);
	glutAddMenuEntry("Menu 2", 1);
	glutAddMenuEntry("Menu 3", 2);
	initOpenGL();

	glutMainLoop();
	return 0;
}
*/

// http://us.battle.net/d3/en/forum/topic/3967848172