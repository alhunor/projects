	#include <iostream>
	#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
	#include <CGAL/Delaunay_triangulation_2.h>
	#include <CGAL/Triangulation_vertex_base_with_info_2.h>

	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
	typedef CGAL::Triangulation_vertex_base_2<K> Vb;
	typedef CGAL::Triangulation_face_base_2<K> Fb;
	typedef CGAL::Triangulation_data_structure_2<Vb,Fb> Tds;
	typedef CGAL::Delaunay_triangulation_2<K,Tds> Triangulation;

	typedef Triangulation::Point Point;
	typedef Triangulation::Triangulation_data_structure TDS;


	using namespace std;


	void main()
	{
		Triangulation t;
		t.insert(Point(0,0));
		t.insert(Point(20,0));
		t.insert(Point(30,15));
		t.insert(Point(30,-15));
		
		Triangulation::Finite_faces_iterator fib = t.finite_faces_begin(), it;
		Triangulation::Finite_faces_iterator fie = t.finite_faces_end();
		Triangulation::Triangle tri;
		std::cout << "Triangular faces"<<endl;
		for (it=fib; it!=fie; ++it)
		{
			tri = t.triangle(it);
			std::cout<<tri[0]<<" "<<tri[1]<<" "<<tri[2]<<" "<<endl;
		}	


	TDS tt = t.tds();
	tt.faces_begin();
	TDS::Face_iterator tdsfib = tt.faces_begin();
	TDS::Face_iterator tdsfie = tt.faces_end();
	TDS::Face_iterator  tdsit;
	TDS::Face f;

	TDS::Vertex_handle vh;

	for (tdsit=tdsfib; tdsit!=tdsfie; ++tdsit)
	{
		f = *tdsit;
		vh = f.vertex(0);
		//vh->info() = CGAL::RED;
	}


		char c;
		std::cin>>c;
	}
