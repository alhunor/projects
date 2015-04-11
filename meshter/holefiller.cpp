#include "holefiller.h"
#include <levmar.h>

using namespace std;

#pragma warning (disable : 4996)

void printOFF(triangulateio& tio)
{
	//std::ostream& io = cout;
	ofstream io;
	io.open("out.off");
	int i,j;

	io<<"OFF"<<std::endl;
	io<< tio.numberofpoints << "  " << tio.numberoftriangles << "   0" << std::endl;
	for (i=0; i<tio.numberofpoints; ++i)
	{
		io<<tio.pointlist[i * 2] << "   " <<tio.pointlist[i * 2+1] << "   0"<<std::endl;
	}

	io<<std::endl;
    for (i = 0; i < tio.numberoftriangles; i++)
	{
		io<<"3  ";
		for (j = 0; j < tio.numberofcorners; j++)
		{
          io<<tio.trianglelist[i * tio.numberofcorners + j]<< " ";
        }
		io<<std::endl;
	}
	io.close();
} // void printOff(triangulateio& tio)


void holeFiller::findHoles()
{
	// TODO delete isolated vertices to be sure


	// iterate through all edges
	unsigned int n = mesh.n_halfedges();
	unsigned int i;
	int* edgeinfo = new int[n];

	for (i=0; i<n; ++i) 
	{
		edgeinfo[i]=-1;
	}

	TriMesh::HalfedgeIter he_it, he_end(mesh.halfedges_end());
	TriMesh::Halfedge H;
	TriMesh::HalfedgeHandle heh, heh_init;

	hehV hole;
	int nbHoles(0);

	for (he_it = mesh.halfedges_begin(); he_it!=he_end; ++he_it)
	{
		heh_init = he_it.handle();	
		//H = mesh.halfedge(heh_init);
		i = heh_init.idx();
//		if (! mesh.is_valid_handle(H.face_handle_)) // boundary
		if (mesh.is_boundary(heh_init)) // boundary
		{
			if (edgeinfo[i]==-1)
			{
				//cout<<"Hole ["<<nbHoles<<"] = "<<heh_init;
				hole.push_back(heh_init);
				edgeinfo[i] = nbHoles;
				// We can do this as often as we want:
				heh = mesh.next_halfedge_handle(heh_init);
				while(heh != heh_init) // go through the boundary edges
				{
					//cout<<" "<<heh;
					hole.push_back(heh);
					edgeinfo[heh.idx()]=nbHoles;
					heh = mesh.next_halfedge_handle(heh);
				}
				++nbHoles;
				//cout<<endl;
				holes.push_back(hole);
				hole.clear(); // get ready for next hole
			}
		} else
		{
			edgeinfo[i]=-1; // this is not a boundary edge
		} // if (! mesh.is_valid_handle(H.face_handle_)) // boundary
	} // for (he_it = mesh.halfedges_begin(); he_it!=he_end; ++he_it)
	delete[] edgeinfo;
} // void findHoles(TriMesh& mesh)


void holeFiller::displayHoles()
{
	unsigned int i, j;
	for(i=0; i<holes.size(); ++i)
	{
		cout<<"Hole "<< i<<endl;
		for(j=0; j<holes[i].size(); ++j)
		{
			// cout<<holes[i][j]<<" "<<mesh.point(mesh.halfedge(holes[i][j]).vertex_handle_)<<endl;
			cout<<holes[i][j]<<" "<<mesh.point(mesh.to_vertex_handle(holes[i][j]))<<endl;
		}
		cout<<endl;
	}
} // void displayHoles(vector<hehV>& holes)

double holeFiller::quality(double * x)
// <boundaryPoints> is the number of points that have been found on the boundary of the initial hole
{
	unsigned int i;
	OpenMesh::Vec3f n;
	OpenMesh::VertexHandle vhandle;
	OpenMesh::Vec3f P;
	double q;
	// set point coordinates in the mesh to x
	for (i=boundaryPoints; i<newPoints.size(); ++i)
	{
		vhandle = newPoints[i].vhandle;
		P = mesh.point(vhandle);
		//change dim3 to x[i-boundaryPoints];
		//P += direction*x[i-boundaryPoints];
		P[dim3] = x[i-boundaryPoints];
		mesh.set_point(vhandle, P);
	}

	// recompute face normals
	for (i=0; i<newFaces.size(); ++i)
	{
		n = mesh.calc_face_normal( newFaces[i] );
		mesh.set_normal(newFaces[i], n );
	}
// recompute vertex normals, make sure to recompute both for initial boundary points and new points added by he hole filling algo
	for (i=0; i<newPoints.size(); ++i)
	{
		n = mesh.calc_vertex_normal(newPoints[i].vhandle);
		newPoints[i].normal = n;
	}
// sum of all vector products between desired and current normals
	q = 0;
	for (i=0; i<newPoints.size(); ++i)
	{
		q += cross(newPoints[i].desirednormal, newPoints[i].normal).norm(); // use norm of vector product here
	}
	return q;
} // double quality(TriMesh& mesh, LightVector<TriMesh::FaceHandle>& newFaces,	LightVector<attributeType>& newPoints)

void holeFiller::computeDim(hehV& hole)
{
	OpenMesh::Vec3f P;
	double sx(0), sx2(0), sy(0), sy2(0), sz(0), sz2(0);
	double stdx(0), stdy(0), stdz(0);
//	struct triangulateio in, mid, out, vorout;
	OpenMesh::VertexHandle vhandle;
	int nbPoints = hole.size();

	for (int i=0; i<nbPoints; ++i)
	{
		vhandle = mesh.to_vertex_handle(hole[i]);
//		vhandle = mesh.halfedge(hole[i]).vertex_handle_;
		P = mesh.point(vhandle );
		sx += P[0];
		sy += P[1];
		sz += P[2];

		sx2 += P[0]*P[0];
		sy2 += P[1]*P[1];
		sz2 += P[2]*P[2];
	}
	stdx += sx2/nbPoints - sq(sx/nbPoints);
	stdy += sy2/nbPoints - sq(sy/nbPoints);
	stdz += sz2/nbPoints - sq(sz/nbPoints);

	if (stdx>stdy)
	{
		if (stdy>stdz)
		{ 
			dim1 = 0; dim2 = 1;
		} else 
		{
			dim1 = 0; dim2 = 2;	
		}
	} else
	{
		if (stdx>stdz)
		{ 
			dim1 = 0; dim2 = 1;
		} else 
		{
			dim1 = 1; dim2 = 2;	
		}
	}
	dim3 = 3 - dim1 - dim2;
}

void qual(double *p, double *x, int m, int n, void *data)
{
	// fills <x> vector for levmar algorithm
	holeFiller* hf = (holeFiller*)data;
	hf->quality(p);
	int i;
	// n == newPoints.size() by constriction
	for(i=0; i<n-m; ++i)
	{
		x[i]=0;
	}

	for(i=n-m; i<n; ++i)
	{
		x[i]=cross(hf->newPoints[i].desirednormal, hf->newPoints[i].normal).norm();
	}
} //void qual(double *p, double *x, int m, int n, void *data)


void holeFiller::fill(int h)
{
// http://www.cs.cmu.edu/~quake/tripaper/triangle3.html

	struct triangulateio in, mid, out, vorout;
	int i,j;
	OpenMesh::VertexHandle vhandle;
	OpenMesh::FaceHandle fhandle;

	hehV& hole = holes[h];

	computeDim(hole);
	OpenMesh::Vec3f P;

	// hole is projected on dim1 x dim2, this is where the stdev of the points is biggest

	in.numberofpoints = hole.size();
	in.numberofpointattributes = 2; // Store 3rd cordinate followed by the vertexHandles from OpenMesh
	in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));
	in.pointattributelist = (REAL *) malloc(in.numberofpoints * in.numberofpointattributes * sizeof(REAL));;
	for (i=0; i<in.numberofpoints; ++i)
	{
		vhandle = mesh.to_vertex_handle(hole[i]);
		//vhandle = mesh.halfedge(hole[i]).vertex_handle_;

		P= mesh.point(vhandle);
		
		in.pointattributelist[2*i] = P[dim3];
		in.pointattributelist[2*i+1] = vhandle.idx();

		in.pointlist[i*2] = P[dim1];
		in.pointlist[i*2+1] = P[dim2];
	}

	in.pointmarkerlist = NULL;

	in.segmentlist = (int*) malloc(in.numberofpoints * 2 * sizeof(int));
	in.segmentmarkerlist = (int*) malloc(in.numberofpoints * sizeof(int));
	for (i=0; i<in.numberofpoints; ++i)
	{
		in.segmentlist[i*2] = i;
		in.segmentlist[i*2+1] = i + 1;
		in.segmentmarkerlist[i] = 1;
	}
	in.segmentlist[in.numberofpoints*2-1] = 0;
	in.numberofsegments = in.numberofpoints;

	in.numberofholes = 0;
	in.numberofregions = 0;
	in.regionlist = NULL;
	in.numberoftriangles = 0;

//Make necessary initializations so that Triangle can return a triangulation in `mid' and a voronoi diagram in `vorout'.

	mid.pointlist = (REAL *) NULL;            // Not needed if -N switch used.
	// Not needed if -N switch used or number of point attributes is zero:
	mid.pointattributelist = (REAL *) NULL;
	mid.pointmarkerlist = (int *) NULL; // Not needed if -N or -B switch used.
	mid.trianglelist = (int *) NULL;          // Not needed if -E switch used.
	// Not needed if -E switch used or number of triangle attributes is zero: 
	mid.triangleattributelist = (REAL *) NULL;
	mid.neighborlist = (int *) NULL;         // Needed only if -n switch used.
	// Needed only if segments are output (-p or -c) and -P not used:
	mid.segmentlist = (int *) NULL;
	// Needed only if segments are output (-p or -c) and -P and -B not used:
	mid.segmentmarkerlist = (int *) NULL;
	mid.edgelist = (int *) NULL;         // Needed only if -e switch used.
	mid.edgemarkerlist = (int *) NULL;   // Needed if -e used and -B not used.

	vorout.pointlist = (REAL *) NULL;        /* Needed only if -v switch used. */
	/* Needed only if -v switch used and number of attributes is not zero: */
	vorout.pointattributelist = (REAL *) NULL;
	vorout.edgelist = (int *) NULL;          /* Needed only if -v switch used. */
	vorout.normlist = (REAL *) NULL;         /* Needed only if -v switch used. */
	
	// Triangulate switches:
	// (p) - read and write a PSLG
	// preserve the convex hull (c)
	// number everything from zero (z)
	// assign a regional attribute to each element (A)
	// produce an edge list (e)
	// a Voronoi diagram (v)
	// and a triangle neighbor list (n).

	triangulate("pzAevn", &in, &mid, &vorout);

	printf("Initial triangulation:\n\n");
	//  report(&mid, 0, 1, 1, 1, 1, 0);
	printOFF(mid);

	// Attach area constraints to the triangles in preparation for refining the triangulation.

	// Needed only if -r and -a switches used:
	mid.trianglearealist = (REAL *) malloc(mid.numberoftriangles * sizeof(REAL));
	mid.trianglearealist[0] = 3.0;
	mid.trianglearealist[1] = 1.0;

	// Make necessary initializations so that Triangle can return a triangulation in `out'.

	out.pointlist = (REAL *) NULL;            // Not needed if -N switch used.
	// Not needed if -N switch used or number of attributes is zero:
	out.pointattributelist = (REAL *) NULL;
	out.trianglelist = (int *) NULL;          // Not needed if -E switch used.
	// Not needed if -E switch used or number of triangle attributes is zero:
	out.triangleattributelist = (REAL *) NULL;

	// Refine the triangulation according to the attached triangle area constraints.

	triangulate("prq22zBP", &mid, &out, (struct triangulateio *) NULL);

	printf("Refined triangulation:\n\n");
	//  report(&out, 0, 1, 0, 0, 0, 0);
	printOFF(out);

	int nbNewPoints = out.numberofpoints - in.numberofpoints;
	int nbNewTriangles = out.numberoftriangles - in.numberoftriangles;
	boundaryPoints = in.numberofpoints;
	newPoints.setSize(out.numberofpoints);
	newFaces.setSize(nbNewTriangles);

	TriMesh::Normal n;
	attributeType iatr;
	// compute vertex normals for boundary points
	for (i=0; i<boundaryPoints; ++i)
	{
		//vhandle = mesh.halfedge(hole[i]).vertex_handle_;
		vhandle = mesh.to_vertex_handle(hole[i]);
		for (TriMesh::ConstVertexFaceIter vf_it=mesh.cvf_iter(vhandle); vf_it; ++vf_it)
			mesh.set_normal(vf_it.handle(), mesh.calc_face_normal(vf_it.handle()) );

		n = mesh.calc_vertex_normal(vhandle);
		P = mesh.point(vhandle );

		iatr.dim3 = dim3;
		iatr.vhandle = vhandle;
		iatr.desirednormal = n;
		newPoints.push_back(iatr);
	}

	// compute desired vertex normals for interior points
	double w, dist, cord3;
	OpenMesh::Vec3f normal;
	double * x0 = NULL;
	if (out.numberofpoints>boundaryPoints)
		x0 = new double[out.numberofpoints-boundaryPoints];

	for (i = boundaryPoints; i<out.numberofpoints; ++i)
	{
		dist = 0;
		cord3 = 0;
		n = OpenMesh::Vec3f(0,0,0);
		for (j=0; j<boundaryPoints; ++j)
		{
			w = 1.0 / (sq(in.pointlist[2*j]-out.pointlist[2*i]) + sq(in.pointlist[2*j+1]-out.pointlist[2*i+1]));
			dist += w;
			cord3 += w* in.pointattributelist[2*j];
			vhandle = OpenMesh::VertexHandle(in.pointattributelist[2*j+1]);
			n += mesh.calc_vertex_normal(vhandle);
		}
		cord3 /= dist;
		out.pointattributelist[2*i] = cord3;
		P[dim1] = out.pointlist[2*i];
		P[dim2] = out.pointlist[2*i+1];
		P[dim3] = cord3;
		vhandle = mesh.add_vertex(P);

		iatr.vhandle = vhandle;
		n.normalize();
		iatr.desirednormal = n;
		iatr.dim3 = cord3;
		x0[i-boundaryPoints] = cord3;

		newPoints.push_back(iatr);
		out.pointattributelist[2*i+1] = vhandle.idx();
	}
	// add new triangles
	int pointA, pointB, pointC;
	for (i = in.numberoftriangles; i < out.numberoftriangles; i++)
	{
		cout<<"Triangle"<< i << "  ";
		pointA = out.trianglelist[i * out.numberofcorners];
		pointB = out.trianglelist[i * out.numberofcorners + 1];
		pointC = out.trianglelist[i * out.numberofcorners + 2];
		cout<<pointA<<"  "<<pointB<<"  "<<pointC<<endl;

		fhandle = addFaceSimple(mesh, OpenMesh::VertexHandle(out.pointattributelist[2*pointA+1]),
							OpenMesh::VertexHandle(out.pointattributelist[2*pointB+1]),
							OpenMesh::VertexHandle(out.pointattributelist[2*pointC+1]));
		newFaces.push_back(fhandle);
	}

	cout<<quality(x0)<<endl;

	double opts[LM_OPTS_SZ], info[LM_INFO_SZ];

	// optimization control parameters; passing to levmar NULL instead of opts reverts to defaults
	opts[0]=LM_INIT_MU;
	opts[1]=1E-15;
	opts[2]=1E-15;
	opts[3]=1E-20;
	opts[4]=LM_DIFF_DELTA; // relevant only if the finite difference Jacobian version is used 

	if (x0!=NULL)
	{
		double *target = new double[out.numberofpoints];
		for (i=0; i<out.numberofpoints; ++i)
		{
			target[i]=0;
		}

	//	ret=dlevmar_dif(expfunc, x0, x, m, n, 1000, opts, info, NULL, NULL, NULL); // without Jacobian
		double ret=dlevmar_dif( qual, x0, target, out.numberofpoints-boundaryPoints, out.numberofpoints, 1000, opts, info, NULL, NULL, this); // without Jacobian
		printf("Levenberg-Marquardt returned in %g iter, reason %g, sumsq %g [%g]\n", info[5], info[6], info[1], info[0]);
		delete[] target;
	}

	// Free all allocated arrays, including those allocated by Triangle.
	delete[] x0;
	free(in.pointlist);
	free(in.pointattributelist);
	free(in.pointmarkerlist);
	free(in.regionlist);
	free(mid.pointlist);
	free(mid.pointattributelist);
	free(mid.pointmarkerlist);
	free(mid.trianglelist);
	free(mid.triangleattributelist);
	free(mid.trianglearealist);
	free(mid.neighborlist);
	free(mid.segmentlist);
	free(mid.segmentmarkerlist);
	free(mid.edgelist);
	free(mid.edgemarkerlist);
	free(vorout.pointlist);
	free(vorout.pointattributelist);
	free(vorout.edgelist);
	free(vorout.normlist);
	free(out.pointlist);
	free(out.pointattributelist);
	free(out.trianglelist);
	free(out.triangleattributelist);
}


/*
void holeFinder::printOff(hehV& hole)
{
	//std::ostream& io = cout;
	ofstream io;
	io.open("outHole.off");
	unsigned int i;

	io<<"OFF"<<std::endl;
	io<< hole.size() << "  " << hole.size() << "   0" << std::endl;

	OpenMesh::Vec3f P;
	for (i=0; i<hole.size(); ++i)
	{
		P = mesh.point(mesh.halfedge(hole[i]).vertex_handle_);
		io<<P[0]<<"  "<<P[1]<<"  "<<P[2]<<endl;
	}
	// writing degenerate triangles as edges
	for (i=0; i<hole.size()-1; ++i)
	{
		io<<"3 "<<i<<" "<<i+1<<" "<< i<< endl;
	}
	io<<"3 "<<0<<" "<<hole.size()-1<<" "<< 0<< endl;

	io.close();
}*/

