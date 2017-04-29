#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Point_set_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/predicates_on_points_2.h>
#include <CGAL/squared_distance_2.h>

//#include <CGAL/predicate_objects_on_points_2.h>

#include <list>
#include <iostream>
#include <fstream>
#include <circulator.h>

using namespace std;


//See http://www.cgal.org/Manual/3.1/doc_html/cgal_manual/Kernel_23/Chapter_extensible_kernel.html

typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef CGAL::Point_set_2<K>::Vertex_handle Vertex_handle;
typedef K::Point_2 Point_2;
typedef K::Vector_2 Vector_2;
typedef K::Vector_3 Vector_3;

typedef CGAL::Polygon_2<K> Polygon_2;


double angle(const Point_2& p, const Point_2& q, const Point_2& r)
{
	Vector_2 P = q-p;
	Vector_2 Q = r-q;
	double cross_product = P.x()*Q.y()-Q.x()*P.y();
	double dot_product = sqrt(P.squared_length()*Q.squared_length());
	return cross_product/dot_product;
}




void rangesearch()
{
	CGAL::Point_set_2<K> PSet;
	std::list<Point_2> Lr;

	Point_2 p1(12,14);
	p1.color = 5;
	Point_2 p2(-12,14);
	Point_2 p3(2,11);
	Point_2 p4(5,6);
	Point_2 p5(6.7,3.8);
	Point_2 p6(11,20);
	Point_2 p7(-5,6);
	Point_2 p8(12,0);
	Point_2 p9(4,31);
	Point_2 p10(-10,-10);

	Lr.push_back(p1); Lr.push_back(p2); Lr.push_back(p3);
	Lr.push_back(p4); Lr.push_back(p5); Lr.push_back(p6);
	Lr.push_back(p7); Lr.push_back(p8); Lr.push_back(p9);
	Lr.push_back(p10);

	PSet.insert(Lr.begin(),Lr.end());

	std::cout << "circular range search !\n";
	std::list<Vertex_handle> LV;
	std::list<Vertex_handle>::const_iterator it;

	CGAL::Circle_2<K> rc(p5,p6);

	PSet.range_search(rc, std::back_inserter(LV));

	
	for (it=LV.begin();it != LV.end(); it++)
		std::cout << (*it)->point() <<"\n";

	std::cout << "triangular range search !\n";

	LV.clear();
	PSet.range_search(p1,p2,p3, std::back_inserter(LV));
	for (it=LV.begin();it != LV.end(); it++)
		std::cout << (*it)->point() << "\n";
	LV.clear();

	std::cout << "isorectangular range search !\n";
	Point_2 pt1=p10;
	Point_2 pt3=p3;
	Point_2 pt2 = Point_2(pt3.x(),pt1.y());
	Point_2 pt4 = Point_2(pt1.x(),pt3.y());

	PSet.range_search(pt1,pt2,pt3,pt4, std::back_inserter(LV));
	for (it=LV.begin();it != LV.end(); it++)
		std::cout << (*it)->point() << "\n";
} //rangesearch


void iterator_test()
{
	list<int> lint;
	for (int i=0; i<4; ++i)
		lint.push_back(i);

	circulator<list<int>::const_iterator> vit(lint.begin(), lint.end());

//	circulator<list<int>::const_iterator> bb(vit);
	do
	{
		cout<<*(vit.curr)<<endl;
		++vit;
	} while (! vit.eof());

} // void iterator_test()


template <class IT>
void iterator_list(IT begin, IT end)
{
	circulator<IT> vit(begin, end);
	do
	{
		cout<<*(vit.curr)<<endl;
		++vit;
	} while (! vit.eof() );

} // void iterator_test()



// http://www.sj-vs.net/c-implementing-const_iterator-and-non-const-iterator-without-code-duplication/
//http://stackoverflow.com/questions/3582608/how-to-correctly-implement-custom-iterators-and-const-iterators
// http://doc.cgal.org/latest/Point_set_processing_3/index.html

void savePolygonToOFF(char* filename, Polygon_2 poly)
{
// http://www.geomview.org/docs/html/OFF.html
	//	ostream& os = cout;
	ofstream os;
	os.open(filename);
	os << "OFF\n";
//	os << "# first line: number of points, number of faces, number of lines"<<endl;
	os << poly.size()<<" "<<poly.size()<<" 0\n"; // number of points, number of faces, number of lines;
//	os << "# list of points: X, Y and Z coordinates"<<endl;

	Polygon_2::Vertex_const_iterator vend = poly.vertices_end();
	Polygon_2::Vertex_const_iterator i;

	for (i=poly.vertices_begin(); i!=vend; ++i)
	{
		os<<i->x() << " "<<i->y() <<" 0\n"; // list of points: X, Y and Z coordinates
	}
	os << "# list of faces: number of points, followed by the sequence number of the composing points (indexed from zero)"<<endl;
	for (unsigned int j=0; j<poly.size(); ++j)
	{
		os<<"3 "<<j<<" "<<(j+1) % poly.size()<<" "<<j<<endl;
	};
	os<<endl;
	os.close();
}

void savePolygonToPLY(char* filename, Polygon_2 poly)
{
	//http://www.cc.gatech.edu/projects/large_models/ply.html
}

void polygon()
{
	list<Point_2> l;
	//l.push_back(Point_2(0,0));
	//l.push_back(Point_2(5.1,0));
	//l.push_back(Point_2(1,1));
	//l.push_back(Point_2(0.5,6));

	ifstream in;
	in.open("hole.xy");
	if (!in.bad())
	{
		K::FT x, y;
		while (!in.eof())
		{
			in>>x>>y;
			l.push_back(Point_2(x,y));
		}
	} else cout<<"Error opening file"<<endl;

	cout<<CGAL::is_simple_2(l.begin(), l.end() )<<endl;
	K::FT result;
	CGAL::area_2(l.begin(), l.end(), result);
	cout<< result<<endl;

	Polygon_2 pgn(l.begin(), l.end());

	cout << "The polygon is " <<(pgn.is_simple() ? "" : "not ") << "simple." << endl;
	cout << "The polygon is " <<(pgn.is_convex() ? "" : "not ") << "convex." << endl;

	
		cout << "The area of the polygon is " <<pgn.area() << endl;
	Point_2 mp(40, 0);
	Point_2 mp2(30, -20);
	Polygon_2::Vertex_const_iterator i = pgn.vertices_begin();
	pgn.set(i, mp);
	pgn.insert(i, mp2);
	cout << "The area of the polygon is " <<pgn.area() << endl;

	cout << "List of points:" <<endl;

	circulator<Polygon_2::Vertex_const_iterator> cc(pgn.vertices_begin(), pgn.vertices_end());
	do
	{
		//cout<<"("<<*cc<<") ";
		cout<<CGAL::squared_distance(*cc.prev, *cc.curr)<<endl;
		++cc;
	} while (!cc.eof());

	cout<<endl;

	iterator_list(pgn.vertices_begin(), pgn.vertices_end());

	savePolygonToOFF("myPoly.off", pgn);
}

/*
void main()
{
//	rangesearch();
	polygon();
//	iterator_test();

	char c; std::cin>>c;
}*/