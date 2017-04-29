// Copyright (c) 1997  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
//
// Author(s)     : Richard Maierhofer 

#ifndef CGAL_CONSTRAINED_TRIANGULATION_FACE_BASE_WITH_INFO_2_H
#define CGAL_CONSTRAINED_TRIANGULATION_FACE_BASE_WITH_INFO_2_H

#include <CGAL/triangulation_assertions.h>
#include <CGAL/Triangulation_ds_face_base_2.h>

namespace CGAL { 

template < typename Info_, class Gt, class Fb = Constrained_triangulation_face_base_2<Gt> >
class Constrained_triangulation_face_base_with_info_2
  :  public Fb
{
  Info_ _info;
public:
  typedef typename Fb::Vertex_handle                   Vertex_handle;
  typedef typename Fb::Face_handle                     Face_handle;
  typedef Info_                                        Info;

  template < typename TDS2 >
  struct Rebind_TDS {
    typedef typename Fb::template Rebind_TDS<TDS2>::Other    Fb2;
    typedef Constrained_triangulation_face_base_with_info_2<Info,Gt,Fb2>    Other;
  };

public:
  const Info& info() const { return _info; }
  Info&       info()       { return _info; }

  Constrained_triangulation_face_base_with_info_2()
	  : Fb()  {}

  Constrained_triangulation_face_base_with_info_2(Vertex_handle v0, 
					Vertex_handle v1, 
					Vertex_handle v2)
    : Fb(v0,v1,v2) {}

  Constrained_triangulation_face_base_with_info_2(Vertex_handle v0, 
					Vertex_handle v1, 
					Vertex_handle v2,
					Face_handle n0, 
					Face_handle n1, 
					Face_handle n2)
    : Fb(v0,v1,v2,n0,n1,n2) {}


  Constrained_triangulation_face_base_with_info_2(Vertex_handle v0, 
					Vertex_handle v1, 
					Vertex_handle v2,
					Face_handle n0, 
					Face_handle n1, 
					Face_handle n2,
					bool c0, 
					bool c1, 
					bool c2 )
    : Fb(v0,v1,v2,n0,n1,n2) {}
  
};
  
} //namespace CGAL 
  
#endif //CGAL_CONSTRAINED_TRIANGULATION_FACE_BASE_WITH_INFO_2_H
