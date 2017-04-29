#ifndef __REGULAR_TRIANGULATION_H_
#define __REGULAR_TRIANGULATION_H_

#include <fstream>
#include <iostream>
#include <list>
#include <cstdlib>
#include <windows.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Constrained_triangulation_face_base_2.h>
//#include <CGAL/Constrained_triangulation_face_base_with_info_2.h>
#include <CGAL/Triangulation_conformer_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Triangulation_conformer_2.h>

#include "octree.h"

template <class T> T* example();

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

class infoT {
public:
	infoT() : inside(false), id(-1), z (0) {}
	bool inside;
	int id;
	float z;
};

//3d projection traits : http://doc.cgal.org/latest/Triangulation_2/Triangulation_2_2terrain_8cpp-example.html


typedef CGAL::Constrained_triangulation_face_base_2<K> Fb_Base; 
typedef CGAL::Triangulation_face_base_with_info_2<int, K, Fb_Base> Fbi; 
typedef CGAL::Triangulation_vertex_base_with_info_2<infoT, K> Vb;
typedef CGAL::Delaunay_mesh_face_base_2<K, Fbi> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb> Tds;
typedef CGAL::Constrained_Delaunay_triangulation_2<K,Tds> Triangulation;
typedef CGAL::Delaunay_mesh_size_criteria_2<Triangulation> Criteria;


typedef Triangulation::Point Point;
typedef Triangulation::Vertex_handle Vertex_handle;
typedef Triangulation::Triangulation_data_structure TDS;
//typedef T::Geom_traits Gt;



Triangulation* example();

//int countOutDomainTriangles(Triangulation& t);


void saveTriangulationToOFF(char* filename, Triangulation& t);

class Voxel
{
public:
	Voxel() : xsize(0), ysize(0), data(NULL) {}
	~Voxel() {if (data) delete[] data; }
	void LoadRaw(char* filename, int resolution, int _xsize, int _ysize); // resolution is number of bits per point
	bool ExportOFF(char* filename);
	int xsize, ysize;
	unsigned short int * data;
};

//http://users.polytech.unice.fr/~lingrand/MarchingCubes/algo.html#squar

//http://0fps.net/2012/07/12/smooth-voxel-terrain-part-2/

//http://doc.cgal.org/latest/Surface_reconstruction_points_3/

// http://www.ics.uci.edu/~dock/manuals/cgal_manual/Triangulation_2/Chapter_main.html

Triangulation* example2(Voxel& v);
//adding Custom info


#endif