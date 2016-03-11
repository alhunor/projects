#ifndef MYKERNEL_H
#define MYKERNEL_H

#include <CGAL/Cartesian.h>
#include <CGAL/basic.h>
#include <CGAL/Filtered_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/squared_distance_2.h>
#include <cassert>

/*#include "MyPointC2.h"
#include "MySegmentC2.h"*/


class MyPointC2 {

private:
  double vec[2];
  int col;

public:

  MyPointC2()
    : col(0)
  {
    *vec = 0;
    *(vec+1) = 0;
  }

  
  MyPointC2(const double x, const double y, int c)
    : col(c)
  {
    *vec = x;
    *(vec+1) = y;
  }

  const double& x() const  { return *vec; }

  const double& y() const { return *(vec+1); }

  double & x() { return *vec; }

  double& y() { return *(vec+1); }

  int color() const { return col; }

  int& color() { return col; }
  
  
  bool operator==(const MyPointC2 &p) const
  {
    return ( *vec == *(p.vec) )  && ( *(vec+1) == *(p.vec + 1) && ( col == p.col) );
  }

  bool operator!=(const MyPointC2 &p) const
  {
      return !(*this == p);
  }

};

template <class ConstructBbox_2>
class MyConstruct_bbox_2 : public ConstructBbox_2 {
public:
  CGAL::Bbox_2 operator()(const typename MyPointC2& p) const {
    return CGAL::Bbox_2(p.x(), p.y(), p.x(), p.y());
  }
};

class MyConstruct_coord_iterator {
public:
  const double* operator()(const MyPointC2& p)
  {
    return &p.x();
  }

  const double* operator()(const MyPointC2& p, int)
  {
    const double* pyptr = &p.y();
    pyptr++;
    return pyptr;
  }
};


 template <typename K>
  class MyConstruct_point_2
  {
    typedef typename K::RT         RT;
    typedef typename K::Point_2    Point_2;
  public:
    typedef Point_2          result_type;
//    typedef CGAL::Arity_tag< 1 >   Arity;

    Point_2
    operator()() const
    { return Point_2(); }

    Point_2
    operator()(CGAL::Origin o) const
    { return Point_2(0,0, 0); }

    Point_2
    operator()(const RT& x, const RT& y) const
    { return Point_2(x, y, 0); }

    
    // We need this one, as such a functor is in the Filtered_kernel
    Point_2
    operator()(const RT& x, const RT& y, const RT& w) const
    { 
      if(w != 1){
	return Point_2(x/w, y/w, 0); 
      } else {
	return Point_2(x,y, 0);
      }
    }
  };




// K_ is the new kernel, and K_Base is the old kernel
template < typename K_, typename K_Base >
class MyCartesian_base : public K_Base::template Base<K_>::Type
{
  typedef typename K_Base::template Base<K_>::Type   OldK;
public:
  typedef K_                                Kernel;
  typedef MyPointC2                         Point_2;
//  typedef MySegmentC2<Kernel>               Segment_2;
  typedef MyConstruct_point_2<Kernel>       Construct_point_2;
  typedef const double*                     Cartesian_const_iterator_2;
  typedef MyConstruct_coord_iterator        Construct_cartesian_const_iterator_2;
  typedef MyConstruct_bbox_2<typename OldK::Construct_bbox_2> 
                                            Construct_bbox_2;

  template < typename Kernel2 >
  struct Base { typedef MyCartesian_base<Kernel2, K_Base>  Type; };
};


template < typename FT_ >
struct MyKernel
  : public MyCartesian_base<MyKernel<FT_>, CGAL::Cartesian<FT_> >
{};

#endif // MYKERNEL_H



// file: examples/Kernel_23/MyKernel.C


typedef MyKernel<double>                   MK;
typedef CGAL::Filtered_kernel_adaptor<MK>  K;
typedef CGAL::Delaunay_triangulation_2<K>  Delaunay_triangulation_2;


typedef K::Point_2         Point;
typedef K::Segment_2       Segment;
typedef K::Ray_2           Ray;
typedef K::Line_2          Line;
typedef K::Triangle_2      Triangle;
typedef K::Iso_rectangle_2 Iso_rectangle;

const int RED= 1;
const int BLACK=2;

int main()
{
  Point a(0,0, RED), b(1,0, BLACK), c(1,1, BLACK), d(0,1, RED);

  Delaunay_triangulation_2 dt;
  dt.insert(a);

  K::Orientation_2 orientation;
  orientation(a,b,c);
  Point p(1,2, BLACK), q;
  p.color() = RED;
  q.color() = BLACK;
  std::cout << p << std::endl;

  K::Compute_squared_distance_2 squared_distance;

  std::cout << "squared_distance(a, b) == "
            << squared_distance(a, b) << std::endl;

  Segment s1(p,q), s2(a, c);

  K::Construct_midpoint_2 construct_midpoint_2;

  construct_midpoint_2(p,q);

  assert(s1.source().color() == RED);

  s1.source().color() = BLACK;

  assert(s1.source().color() == BLACK);

  K::Intersect_2 intersection;

  CGAL::Object o = intersection(s1, s2);

  K::Construct_cartesian_const_iterator_2 construct_it;
  K::Cartesian_const_iterator_2  cit = construct_it(a);
  assert(*cit == a.x());

  cit = construct_it(a,0);

  cit--;
  assert(*cit == a.y());

  Line l1(a,b), l2(p, q);

  intersection(l1, l2);


  intersection(s1, l1);

  Ray r1(d,b), r2(d,c);
  intersection(r1, r2);

  intersection(r1, l1);

  squared_distance(r1, r2);
  squared_distance(r1, l2);
  squared_distance(r1, s2);

  Triangle t1(a,b,c), t2(a,c,d);
  intersection(t1, t2);
  intersection(t1, l1);

  intersection(t1, s1);

  intersection(t1, r1);

  Iso_rectangle i1(a,c), i2(d,p);
  intersection(i1, i2);
  intersection(i1, s1);

  intersection(i1, r1);
  intersection(i1, l1);

  t1.orientation();

  std::cout << s1.source() << std::endl;

  std::cout << t1.bbox() << std::endl;
  return 0;
}
