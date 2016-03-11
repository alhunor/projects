#include "triangulation.h"


using namespace std;


Triangulation* example()
{
	Triangulation* t = new Triangulation();
	//	std::ifstream in("triangulation_prog1.cin");
	//std::ifstream in("kite.in");
	//std::istream_iterator<Point> begin(in);
	//std::istream_iterator<Point> end;
	//t->insert(begin, end);

	Vertex_handle va = t->insert(Point(0,0));
	Vertex_handle vb = t->insert(Point(35,15));
	Vertex_handle vc = t->insert(Point(20,0));
	Vertex_handle vd = t->insert(Point(30,-15));


	t->insert_constraint(va,vb);
	t->insert_constraint(vb,vc);
	t->insert_constraint(vc,vd);
	t->insert_constraint(vd,va);


	// more efficiently
	CGAL::Triangulation_conformer_2<Triangulation> tc(*t);
	tc.init_Delaunay ();
	cout << "Number of vertices before: "<< t->number_of_vertices() << std::endl;
	tc.make_conforming_Delaunay();
	cout << "Number of vertices after make_conforming_Delaunay_2: "<< t->number_of_vertices() << endl;
 	tc.make_conforming_Gabriel ();
	cout << "Number of vertices after make_conforming_Gabriel_2: "<< t->number_of_vertices() << endl;


	std::list<Point> list_of_seeds;
//	list_of_seeds.push_back(Point(21, 0)); //seeds are used to define holes
	
	CGAL::refine_Delaunay_mesh_2(*t, list_of_seeds.begin(), list_of_seeds.end(), Criteria(0.25,10));
	cout << "Number of vertices after refine_Delaunay_mesh_2: " << t->number_of_vertices() << endl;

	Triangulation::Face_handle fh;
	for(Triangulation::Finite_faces_iterator it = t->finite_faces_begin();  it != t->finite_faces_end() ; ++it)
	{
		fh= it;
//		cout<<it->is_in_domain()<<endl;
//		if (! it->is_in_domain())
//			t->delete_face(it);
	}
	return t;
}




int countOutDomainTriangles(Triangulation& t)
{
	int count=0;
	Triangulation::Finite_faces_iterator fie = t.finite_faces_end();
	for (Triangulation::Finite_faces_iterator it = t.finite_faces_begin(); it!=fie; ++it)
	{
		if (!it->is_in_domain())
		{
			++count;
		}
	}
	return count;
} //countInsideTriangles(Triangulation& t)


void saveTriangulationToOFF(char* filename, Triangulation& t)
{
//	ostream& os = cout;
	ofstream os;
	os.open(filename);

	os << "OFF\n";
	os << t.number_of_vertices()<<" "<< t.number_of_faces() - countOutDomainTriangles(t)<<" 0\n"; // number of points, number of faces, number of lines;
	CGAL::Unique_hash_map<Triangulation::Vertex_handle,int> V;

	// vertices
	Point p;
	int inum = 0;
	if (t.number_of_vertices() >1 )
	{
		for(Triangulation::Vertex_iterator vit = t.vertices_begin(); vit != t.vertices_end() ; ++vit)
		{
			if (vit->info().id<0) vit->info().id = inum;
			//cout<<vit->info().inside<<endl;
			V[vit] = inum;
			p = vit->point();
			os << p.x() <<" "<< p.y() <<" "<<vit->info().z<<std::endl;
			inum++;
		}
	}
	os << "# list of faces: number of points, followed by the sequence number of the composing points (indexed from zero)"<<endl;

	// faces
	int dim = 2;

	Triangulation::Finite_faces_iterator fib = t.finite_faces_begin();
	Triangulation::Finite_faces_iterator fie = t.finite_faces_end();
	Triangulation::Finite_faces_iterator it;
	Triangulation::Triangle tri;
	Triangulation::Vertex_handle vh;

	for (it=fib; it!=fie; ++it)
	{
		//if (it->is_in_domain())
		{
			os <<"3 ";
			//tri = t.triangle(it);
			vh = it->vertex(0);
			os<<V[it->vertex(0)]<<" "<<V[it->vertex(1)]<<" "<<V[it->vertex(2)]<<" "<<endl;
		}
	}
	os.close();
} // void saveTriangulationToOFF(char* filename, Triangulation& t)


void Voxel::LoadRaw(char* filename, int bytesPerPoint, int _xsize, int _ysize)
{
	DWORD FileSizeHigh;
	OFSTRUCT ReOpenBuff;
	HANDLE hFile;
	unsigned short int Buffer[16384];
	int minh(65000), maxh(0);
	if (bytesPerPoint !=2) throw "Invalid resolution (must have bytesPerPoint = 2).";
	try
	{
		hFile = (HANDLE)OpenFile( filename,   &ReOpenBuff, OF_READ );
		DWORD filesize = GetFileSize(hFile, &FileSizeHigh);
		if (_xsize * _ysize* bytesPerPoint != filesize) throw "Invalid file size.";
		xsize = _xsize;
		ysize = _ysize;
		data = new unsigned short int [xsize * ysize];
		DWORD NumberOfBytesRead;
		unsigned int i, inbuffer = 0;
		do
		{
			ReadFile(hFile, &Buffer, 32768, &NumberOfBytesRead, NULL);
			for (i=0; i<NumberOfBytesRead/2; ++i)
			{
				data[inbuffer++]=Buffer[i];
				if (Buffer[i]<minh) minh=Buffer[i];
				if (Buffer[i]>maxh) maxh=Buffer[i];
			}
		} while (NumberOfBytesRead>0);
	} catch (...)
	{
		CloseHandle(hFile);
	}
	cout<<minh<<"  "<< maxh<<endl;
} //void Voxel::LoadRaw(char* filename, int resolution, int _xsize, int _ysize)


bool Voxel::ExportOFF(char* filename)
{
	ofstream os;
	os.open(filename);

	os << "OFF\n";
	os << xsize*ysize <<" "<< (xsize-1)*(ysize-1)<<" 0\n"; // number of points, number of faces, number of lines;
	// Points
	int nbPoints = xsize * ysize;
	for (int i=0; i<nbPoints; ++i)
	{
		os<<i/xsize <<" "<<i % xsize<<"  "<<(float)data[i]/4096 << endl;
	}
	// Triangles
	int xsizePlusOne = xsize+1;
	for (int i=0; i<nbPoints-xsize; ++i)
	{
		if (i % xsize <xsize-1)
		{
			if ( rand() % 2==1)
			{
				os<<"3 "<< i << " "<<i+1<<"  "<<i+xsize << endl;
				os<<"3 "<< i+1 << " "<<i+xsizePlusOne <<"  "<<i+xsize << endl;
			} else
			{
				os<<"3 "<< i << " "<<i+1<<"  "<<i+xsizePlusOne << endl;
				os<<"3 "<< i << " "<<i+xsizePlusOne <<"  "<<i+xsize << endl;
			}
		}
	}
	
	return true;
} //bool Voxel::ExportOFF(char* filename)

Triangulation* example2(Voxel& v)
{
	Triangulation* t = new Triangulation();
	//	std::ifstream in("triangulation_prog1.cin");
	//std::ifstream in("kite.in");
	//std::istream_iterator<Point> begin(in);
	//std::istream_iterator<Point> end;
	//t->insert(begin, end);
	
	Vertex_handle vh;
	int i,j,k;
	for (i=0; i<10; ++i)
		for (j=0; j<10; ++j)
		{
			k = i*v.xsize + j;
			vh = t->insert(Point(i, j));
			vh->info().z = (float)v.data[k] / 4096;
		}
	//for (int i=0; i<v.xsize*v.ysize; ++i)
	//{
	//	vh = t->insert(Point(i / v.xsize, i % v.xsize));
	//	vh->info().z = v.data[i];
	//}

/*	CGAL::Triangulation_conformer_2<Triangulation> tc(*t);
	tc.init_Delaunay ();
	cout << "Number of vertices before: "<< t->number_of_vertices() << std::endl;
	tc.make_conforming_Delaunay();
	cout << "Number of vertices after make_conforming_Delaunay_2: "<< t->number_of_vertices() << endl;
	*/
	return t;
}
