//=============================================================================
//                                                
//   Code framework for the lecture
//
//   "Surface Representation and Geometric Modeling"
//
//   Mark Pauly, Mario Botsch, Balint Miklos, and Hao Li

//
//  CLASS ValenceViewer
//
//=============================================================================


#ifndef VALENCEVIEWERWIDGET_HH
#define VALENCEVIEWERWIDGET_HH


//== INCLUDES =================================================================


#include "MeshViewer.hh"

//== DEFINES AND CONSTANTS ====================================================

#define NUMBER_OF_COLORS 10

//== CLASS DEFINITION =========================================================

	      

class ValenceViewer : public MeshViewer
{
public:
   
  /// default constructor
  ValenceViewer(const char* _title, int _width, int _height);

  // destructor
  ~ValenceViewer();

  /// open mesh
  virtual bool open_mesh(const char* _filename);



protected:
  //Properties for the vertex valence computation
  OpenMesh::VPropHandleT<int> vertexValence;
  Mesh::Color vertexColors[NUMBER_OF_COLORS];
  virtual void draw(const std::string& _draw_mode);

  /// calculate vertex valences
  void calc_valences();

  /// set vertex color from vertex valence
  void color_coding();
  void InitializeColorsArray();

private:

};


//=============================================================================
#endif // VALENCEVIEWERWIDGET_HH defined
//=============================================================================

