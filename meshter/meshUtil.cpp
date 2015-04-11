#pragma  warning (disable:4244) // loss of precision when converting double to float
#pragma  warning (disable:4305) // truncation from 'double' to 'const float'
#pragma  warning (disable:4996) // truncation from 'double' to 'const float'



#include "meshutil.h"
#include <cmath>
const double M_sin60 = sqrt(3.0)/2;

template<typename T>
inline T min(T a, T b)
{
	if (a<b) return a; else return b;
}

template<typename T>
inline T min3(T a, T b, T c)
{
	if (a<b) return min(a,c); else return min(b,c);
}

template<typename T>
inline void swap(T& a, T& b)
{
	T c = a;
	a = b;
	b = c;
}


MyREAL angle(OpenMesh::Vec3f& pointA, OpenMesh::Vec3f& pointB, OpenMesh::Vec3f& pointC)
{
	MyREAL sinTheta = cross(pointA-pointB, pointC-pointB).norm() / ((pointA-pointB).norm()*(pointC-pointB).norm());
	MyREAL cosTheta = dot(pointA-pointB, pointC-pointB);
	MyREAL theta = asin(sinTheta)*PITOGRAD; 
	if (cosTheta<0)
	{
		theta = 360 - theta;
	}
	return theta;
}

// check if there is already an edge from a->b
inline bool is_valid_edge(TriMesh& mesh, TriMesh::VertexHandle a, TriMesh::VertexHandle b)
{
	TriMesh::HalfedgeHandle heh = mesh.halfedge_handle(a);
	if (mesh.is_valid_handle (heh))
	{
//		return (mesh.halfedge(heh).vertex_handle_ == b);
		return mesh.to_vertex_handle(heh) == b;
	}
	// we reach this point is there is no halfedge starting from a
	return false;
}



bool acceptableFace(TriMesh& mesh, TriMesh::VertexHandle a, TriMesh::VertexHandle b, TriMesh::VertexHandle c)
{
	if ( ! (mesh.is_boundary(a) && mesh.is_boundary(b) && mesh.is_boundary(c)))
	{
		std::cout<<"acceptableFace: Vertex is not on the boundary\n";
		return false;
	}

	// Initialise edge attributes
	TriMesh::HalfedgeHandle halfedge_handle = mesh.find_halfedge(a, b);
	if (halfedge_handle.is_valid() && !mesh.is_boundary(halfedge_handle))
	{
		//std::cout<<"acceptableFace: Edge 1 is not on the boundary\n";
		return false;
	}
	halfedge_handle = mesh.find_halfedge(b, c);
	if (halfedge_handle.is_valid() && !mesh.is_boundary(halfedge_handle))
	{
		//std::cout<<"acceptableFace: Edge 2 is not on the boundary\n";
		return false;
	}
	halfedge_handle = mesh.find_halfedge(c, a);
	if (halfedge_handle.is_valid() && !mesh.is_boundary(halfedge_handle))
	{
		//std::cout<<"acceptableFace: Edge 3 is not on the boundary\n";
		return false;
	}
	return true;
}



TriMesh::FaceHandle addFaceUnordered(TriMesh& mesh, TriMesh::VertexHandle a, TriMesh::VertexHandle b, TriMesh::VertexHandle c)
// adds a new face to the mesh, if the verteces are already part of the mesh
// vertices can be given in any order
{
	if (! mesh.is_valid_handle (a) || ! mesh.is_valid_handle (b) ||! mesh.is_valid_handle (c))
	{
		std::cout<<"Problem."<<std::endl;
	}

	if (! acceptableFace(mesh,a,b,c))
	{
		bool bb = acceptableFace(mesh, b, a, c);
		swap(a,b);
	}
	
//	std::cout<< "Added "<<a<<"  "<<b<<"  "<<c<<std::endl;
	TriMesh::FaceHandle fh = mesh.add_face(a, b, c);
	return fh;
}

TriMesh::FaceHandle addFaceSimple(TriMesh& mesh, TriMesh::VertexHandle a, TriMesh::VertexHandle b, TriMesh::VertexHandle c)
// adds a new face to the mesh, if the verteces are already part of the mesh
{
	TriMesh::FaceHandle fh = mesh.add_face(a, b,c );
	if (fh.idx()<0) // invalid face
		fh = mesh.add_face(b, a,c ); // reorder the points
	return fh;
} // addFaceSimple




void addFace(TriMesh& mesh, TriMesh::VertexHandle a, TriMesh::VertexHandle b, TriMesh::VertexHandle c)
// adds a new face to the mesh, if the verteces are already part of the mesh
{
	static std::vector<TriMesh::VertexHandle>  face_vhandles(3);
// compute quality
	OpenMesh::Vec3f pointA = mesh.point(a);
	OpenMesh::Vec3f pointB = mesh.point(b);
	OpenMesh::Vec3f pointC = mesh.point(c);

	MyREAL angleA = angle(pointC, pointA, pointB);
	MyREAL angleB = angle(pointA, pointB, pointC);
	MyREAL angleC = angle(pointB, pointC, pointA);
	MyREAL ab = (pointA-pointB).norm();
	MyREAL bc = (pointB-pointC).norm();
	MyREAL ca = (pointC-pointA).norm();

	OpenMesh::Vec3f av = pointA-pointC;
	OpenMesh::Vec3f bv = pointB-pointC;
	// http://en.wikipedia.org/wiki/Circumcenter
	MyREAL circumdiameter = av.norm()*bv.norm()*(av-bv).norm() / cross(av,bv).norm();
//	double quality = min3(angleA, angleB, angleC);
	MyREAL quality = min3(ab, bc, ca) / circumdiameter; // bad if quality < Threshold
//	cout<< "Adding" << a <<"  " << b << "  "<< c<<"  "<<quality<<endl;

//	if (quality<threshold) addFaceToListofFutureImprovements;
	addFaceSimple(mesh, a,b,c);
} // addFace

// deletes the triangle identified by <f> and adds a new tetrahedron to the mesh formed by the vertices of the deleted tringle and the new point <P>
void extendMesh(TriMesh& mesh, TriMesh::FaceHandle& fh, OpenMesh::Vec3f& P, bool runGarbageCollector)
{

	OpenMesh::VertexHandle newP = mesh.add_vertex(P);
	TriMesh::ConstFaceVertexIter cfvIt;
	cfvIt = mesh.cfv_iter(fh);
	OpenMesh::VertexHandle pointA = cfvIt.handle();
	OpenMesh::VertexHandle pointB = (++cfvIt).handle();
	OpenMesh::VertexHandle pointC = (++cfvIt).handle();
	mesh.delete_face(fh, false);
	addFace(mesh, pointA, pointB, newP);
	addFace(mesh, pointB, pointC, newP);
	addFace(mesh, pointC, pointA, newP);
	if (runGarbageCollector)
	{
		mesh.garbage_collection();
	}
}


void subdivide(TriMesh& mesh, int k)
{

	// Initialize subdivider
	OpenMesh::Subdivider::Uniform::LoopT<TriMesh> loop;
	loop.attach(mesh);
	loop( k , false);
	loop.detach();
} // void subdivide()


TriMesh createTetra(MyREAL edgelen)
{
	TriMesh mesh;
	TriMesh::VertexHandle vhandle[4];
	vhandle[0] = mesh.add_vertex(TriMesh::Point(edgelen/2, 0,  -edgelen*sqrt(2.0)/4.0));
	vhandle[1] = mesh.add_vertex(TriMesh::Point(-edgelen/2, 0,  -edgelen*sqrt(2.0)/4.0));
	vhandle[2] = mesh.add_vertex(TriMesh::Point( 0,  -edgelen/2,  edgelen*sqrt(2.0)/4.0));
	vhandle[3] = mesh.add_vertex(TriMesh::Point(0,  edgelen/2,  edgelen*sqrt(2.0)/4.0));

	//std::vector<TriMesh::VertexHandle>  face_vhandles;

	addFace(mesh, vhandle[1], vhandle[2], vhandle[0]);
	addFace(mesh, vhandle[0], vhandle[2], vhandle[3]);
	addFace(mesh, vhandle[3], vhandle[2], vhandle[1]);
	addFace(mesh, vhandle[1], vhandle[0], vhandle[3]);

	return mesh;
}// TriMesh createTetra()



TriMesh createSphere(MyREAL radius, int subdivisionLevel)
// creates a sphere centred in (0,0,0) with radius "radius" 
// See http://en.wikipedia.org/wiki/Tetrahedron for radius of circumsphere formula
{
	MyREAL tetraEdge = 4 * radius / sqrt(6.0);
	TriMesh mesh = createTetra(tetraEdge); // OpenMesh::IO::write_mesh(mesh, "tetra.off"); readmesh(mesh, "tetra.off");
	subdivide(mesh, subdivisionLevel);
	OpenMesh::Vec3f O(0,0,0);
// iterate through all vertices and push them to the sphere
	TriMesh::VertexIter v_it, v_end(mesh.vertices_end());
	OpenMesh::Vec3f P;
	for (v_it = mesh.vertices_begin(); v_it!=v_end; ++v_it)
	{
        P = mesh.point(v_it);
		P *= radius / P.norm();
		//cout<<P<<" --> "<<P.norm()<<endl;
		mesh.set_point(v_it, P);
	}
	return mesh;
}



TriMesh createTorus(MyREAL bigR, MyREAL smallr, unsigned int k)
{
	if (k % 2) ++k; // make sure k is always even, or else the last ring wont form a perfect hexagon and may look ugly.
	unsigned int n = bigR / smallr * k * M_sin60 + 0.5; // number of triangles covering the big circle
	MyREAL bigalpha, smallalpha; // angular coordiantes of the points to be generated in the big and small circles
	MyREAL bigalphaInc = 2*M_PI / n;
	MyREAL smallalphaInc = 2*M_PI / k;

	// generate points on the bigest circle
	TriMesh mesh;
	unsigned int i, j;
	TriMesh::VertexHandle* pbig = new TriMesh::VertexHandle[n+1];// points on the big circle
	TriMesh::VertexHandle* points[2];
	points[0] = new TriMesh::VertexHandle[n+1];// points on the big circle;
	points[1] = new TriMesh::VertexHandle[n+1];// points on the big circle;
	MyREAL x,y,z;

	smallalpha = 0;
	bigalpha = 0;
	for (i = 0; i<n; ++i) 
	{
		x = (bigR + smallr*cos(smallalpha) ) * cos (bigalpha+bigalphaInc/2);
		y = (bigR + smallr*cos(smallalpha) ) * sin (bigalpha+bigalphaInc/2);
		z = smallr*sin(smallalpha);
		pbig[i] = mesh.add_vertex(TriMesh::Point(x,y,z));
		points[0][i] = pbig[i];
		bigalpha += bigalphaInc;
	}

	for (j = 1; j<k; ++j) 
	{
		smallalpha += smallalphaInc;
		bigalpha = 0;
		if (j % 2) // shift vertex positions for odd values
		{
			x = (bigR + smallr*cos(smallalpha) ) * cos (bigalpha);
			y = (bigR + smallr*cos(smallalpha) ) * sin (bigalpha);
		} else
		{
			x = (bigR + smallr*cos(smallalpha) ) * cos (bigalpha+bigalphaInc/2);
			y = (bigR + smallr*cos(smallalpha) ) * sin (bigalpha+bigalphaInc/2);
		}
		z = smallr*sin(smallalpha);
		points[j % 2 ][0] = mesh.add_vertex(TriMesh::Point(x,y,z));

		for (i = 1; i<n; ++i) 
		{
			bigalpha += bigalphaInc;
			if (j % 2) // shift vertex positions for odd values
			{
				x = (bigR + smallr*cos(smallalpha) ) * cos (bigalpha);
				y = (bigR + smallr*cos(smallalpha) ) * sin (bigalpha);
			} else
			{
				x = (bigR + smallr*cos(smallalpha) ) * cos (bigalpha+bigalphaInc/2);
				y = (bigR + smallr*cos(smallalpha) ) * sin (bigalpha+bigalphaInc/2);
			}
			z = smallr*sin(smallalpha);
			points[j % 2 ][i] = mesh.add_vertex(TriMesh::Point(x,y,z));
			if (j % 2)
			{
				addFaceUnordered(mesh, points[j % 2 ][i-1], points[j % 2 ][i], points[(j + 1) % 2][i-1]);
				addFaceUnordered(mesh, points[j % 2 ][i], points[(j + 1) % 2][i], points[(j+1) % 2 ][i-1]);
			} else
			{
				addFaceUnordered(mesh, points[j % 2 ][i-1], points[(j + 1) % 2 ][i], points[(j + 1) % 2][i-1]);
				addFaceUnordered(mesh, points[j % 2 ][i], points[(j + 1) % 2][i], points[j % 2 ][i-1]);
			}
		}
		if (j % 2)
		{
			addFaceUnordered(mesh, points[j % 2 ][n-1], points[j % 2 ][0], points[(j + 1) % 2][n-1]);
			addFaceUnordered(mesh, points[j % 2 ][0], points[(j + 1) % 2][0], points[(j+1) % 2 ][n-1]);
		} else
		{
			addFaceUnordered(mesh, points[(j+1) % 2 ][n-1], points[(j+1) % 2 ][0], points[j % 2][n-1]);
			addFaceUnordered(mesh, points[j % 2 ][0], points[j % 2][n-1], points[(j+1) % 2 ][0]);
		}
	}
	// points[(k-1) % 2 ][..] is the last populated ring
	// The initial set of point in pbig[] needs to be linked with points[k % 2 ][..]
	if (k % 2)
	{
		for (i = 0; i<n; ++i) 
		{
			addFaceUnordered(mesh, pbig[i], pbig[(i+1)%n], points[(k-1) % 2 ][i]);
			addFaceUnordered(mesh, pbig[(i+1)%n], points[(k-1) % 2][i], points[(k-1) % 2][(i+1)%n]);
		}
	} else
	{
		for (i = 0; i<n; ++i) 
		{
			addFaceUnordered(mesh, pbig[i], points[(k-1) % 2 ][(i+1)%n], points[(k-1) % 2 ][i]);
			addFaceUnordered(mesh, pbig[(i+1)%n], pbig[i], points[(k-1) % 2][(i+1)%n]);
		}
	}
	delete[] points[1];
	delete[] points[0];
	delete[] pbig;
	return mesh;
}