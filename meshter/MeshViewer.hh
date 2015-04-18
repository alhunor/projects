//=============================================================================
//                                                
//   Code framework for the lecture
//
//   "Surface Representation and Geometric Modeling"
//
//   Mark Pauly, Mario Botsch, Balint Miklos, and Hao Li


#ifndef MESH_VIEWER_WIDGET_HH
#define MESH_VIEWER_WIDGET_HH


//== INCLUDES =================================================================


#include "GlutExaminer.hh"
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>


//== CLASS DEFINITION =========================================================


class MeshViewer : public GlutExaminer
{
public:
   
  /// default constructor
  MeshViewer(const char* _title, int _width, int _height);

  /// open mesh
  virtual bool open_mesh(const char* _filename);

  /// update buffer with face indices
  void update_face_indices();

  /// draw the scene
  virtual void draw(const std::string& _draw_mode);
  

protected:

  
  struct ValenceTraits : public OpenMesh::DefaultTraits
  {
	  VertexTraits
	  {
	  public:
		  int valence;
	  };
  };

  typedef OpenMesh::TriMesh_ArrayKernelT<ValenceTraits>  Mesh;
protected:

  Mesh                       mesh_;
  std::vector<OpenMesh::VertexHandle>  indices_;
};


//=============================================================================
#endif // MESH_VIEWER_WIDGET_HH defined
//=============================================================================

