#ifndef __REGULAR_TRIANGULATION_H_
#define __REGULAR_TRIANGULATION_H_

// Triangulation_2/triangulation_prog1.cpp
#include <fstream>
#include <iostream>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/Triangulation_2.h>
#include <CGAL/Regular_triangulation_euclidean_traits_2.h>
#include <CGAL/Regular_triangulation_filtered_traits_2.h>
#include <CGAL/Regular_triangulation_2.h>
//#include <CGAL/Triangulation_vertex_base_with_info_2.h>


typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef CGAL::Regular_triangulation_filtered_traits_2<K>  Traits;
typedef CGAL::Regular_triangulation_vertex_base_2<Traits> RVb;
typedef CGAL::Regular_triangulation_face_base_2<Traits> RFb;
typedef CGAL::Triangulation_data_structure_2<RVb, RFb> RTds;
typedef CGAL::Regular_triangulation_2<Traits, RTds> Regular_triangulation;
//typedef CGAL::Regular_triangulation_2<Traits> Regular_triangulation;


//typedef Regular_triangulation T;
template <class T> T* example();

template <>
Regular_triangulation* example<Regular_triangulation>()
{
	std::ifstream in("triangulation_prog1.cin");

//	std::ifstream in("regular.cin");
	Regular_triangulation::Weighted_point wp;
	int count = 0;
	std::vector<Regular_triangulation::Weighted_point> wpoints;
	while(in >> wp)
	{
		count++;
		wpoints.push_back(wp);
	}
	Regular_triangulation* rt = new Regular_triangulation(wpoints.begin(), wpoints.end());
	rt->is_valid();
	std::cout << "number of inserted points : " << count << std::endl;
	std::cout << "number of vertices :  " << rt->number_of_vertices() << std::endl;
	std::cout << "number of hidden vertices :  " << rt->number_of_hidden_vertices() << std::endl;
	return rt;
}
#endif