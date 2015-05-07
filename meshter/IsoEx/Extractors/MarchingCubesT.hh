//=============================================================================
//
//  CLASS MarchingCubesT
//
//=============================================================================

#ifndef ISOEX_MARCHINGCUBEST_HH
#define ISOEX_MARCHINGCUBEST_HH

//== INCLUDES =================================================================

#include <IsoEx/Extractors/Edge2VertexMapT.hh>
#include <IsoEx/Extractors/MCTables.hh>
#include <IsoEx/Grids/Grid.hh>
#include <map>

//== NAMESPACES ===============================================================

namespace IsoEx {

//== CLASS DEFINITION =========================================================


/** \class MarchingCubesT MarchingCubesT.hh <IsoEx/Extractors/MarchingCubesT.hh>
    This class implements the well known Marching Cubes algorithm.
    The 0-level iso-surface is extracted in the constructor. Use it through the convenience function 
    <b> IsoEx::marching_cubes() </b>.
    \ingroup extractors
*/	      
template <class Mesh>
class MarchingCubesT
{
public:
   
  MarchingCubesT(const Grid& _grid, Mesh& _mesh);

  
private:

  typedef Grid::PointIdx      PointIdx;
  typedef Grid::CubeIdx       CubeIdx;
  typedef Grid::CubeIterator  CubeIterator;

  typedef typename Mesh::Point         Point;
  typedef typename Mesh::VertexHandle  VertexHandle;
  typedef typename Mesh::FaceHandle    FaceHandle;


  void process_cube(CubeIdx _idx);
  VertexHandle add_vertex(PointIdx _p0, PointIdx _p1);


  const Grid&      grid_;
  Mesh&            mesh_;

  // maps an edge to the sample vertex generated on it
  Edge2VertexMapT<PointIdx, VertexHandle> edge2vertex_;
};


//-----------------------------------------------------------------------------


/** Convenience wrapper for the Marching Cubes algorithm.
    \see IsoEx::MarchingCubesT
    \ingroup extractors
*/	      
template <class Mesh>
void marching_cubes(const Grid& _grid, Mesh& _mesh)
{
  MarchingCubesT<Mesh> mc(_grid, _mesh);
}


template <class Mesh>
MarchingCubesT<Mesh>::
MarchingCubesT(const Grid& _grid, Mesh& _mesh)
  : grid_(_grid),
    mesh_(_mesh)
{
  CubeIterator cube_it(grid_.begin()), cube_end(grid_.end());
  for (; cube_it!=cube_end; ++cube_it)
    process_cube(*cube_it);
}


//-----------------------------------------------------------------------------


template <class Mesh>
void
MarchingCubesT<Mesh>::
process_cube(CubeIdx _cidx)
{
  PointIdx           corner[8];
  VertexHandle       samples[12];
  unsigned char      cubetype(0);
  unsigned int       i;


  // get point indices of corner vertices
  for (i=0; i<8; ++i)
    corner[i] = grid_.point_idx(_cidx, i);


  // determine cube type
  for (i=0; i<8; ++i)
    if (grid_.scalar_distance(corner[i]) > 0.0)
      cubetype |= (1<<i);


  // trivial reject ?
  if (cubetype == 0 || cubetype == 255)
    return;


  // compute samples on cube's edges
  if (edgeTable[cubetype]&1)    samples[0]  = add_vertex(corner[0], corner[1]);
  if (edgeTable[cubetype]&2)    samples[1]  = add_vertex(corner[1], corner[2]);
  if (edgeTable[cubetype]&4)    samples[2]  = add_vertex(corner[3], corner[2]);
  if (edgeTable[cubetype]&8)    samples[3]  = add_vertex(corner[0], corner[3]);
  if (edgeTable[cubetype]&16)   samples[4]  = add_vertex(corner[4], corner[5]);
  if (edgeTable[cubetype]&32)   samples[5]  = add_vertex(corner[5], corner[6]);
  if (edgeTable[cubetype]&64)   samples[6]  = add_vertex(corner[7], corner[6]);
  if (edgeTable[cubetype]&128)  samples[7]  = add_vertex(corner[4], corner[7]);
  if (edgeTable[cubetype]&256)  samples[8]  = add_vertex(corner[0], corner[4]);
  if (edgeTable[cubetype]&512)  samples[9]  = add_vertex(corner[1], corner[5]);
  if (edgeTable[cubetype]&1024) samples[10] = add_vertex(corner[2], corner[6]);
  if (edgeTable[cubetype]&2048) samples[11] = add_vertex(corner[3], corner[7]);



  // connect samples by triangles
  for (i=0; triTable[cubetype][0][i] != -1; i+=3 )
    mesh_.add_face(samples[triTable[cubetype][0][i  ]],
		   samples[triTable[cubetype][0][i+1]],
		   samples[triTable[cubetype][0][i+2]]);
}


//-----------------------------------------------------------------------------


template <class Mesh>
typename MarchingCubesT<Mesh>::VertexHandle
MarchingCubesT<Mesh>::
add_vertex(PointIdx _p0, PointIdx _p1)
{
  // find vertex if it has been computed already
  VertexHandle   vh = edge2vertex_.find(_p0, _p1);
  if (vh.is_valid())  return vh;



  // generate new vertex
  const OpenMesh::Vec3f&  p0(grid_.point(_p0));
  const OpenMesh::Vec3f&  p1(grid_.point(_p1));

  float s0 = fabs(grid_.scalar_distance(_p0));
  float s1 = fabs(grid_.scalar_distance(_p1));
  float t  = s0 / (s0+s1);

  vh = mesh_.add_vertex((1.0f-t)*p0 + t*p1);
  edge2vertex_.insert(_p0, _p1, vh);

  return vh;
}



//=============================================================================
} // namespace IsoEx
//=============================================================================

//=============================================================================
#endif // ISOEX_MARCHINGCUBEST_HH defined
//=============================================================================
