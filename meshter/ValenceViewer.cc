//=============================================================================
//                                                
//   Code framework for the lecture
//
//   "Surface Representation and Geometric Modeling"
//
//   Mark Pauly, Mario Botsch, Balint Miklos, and Hao Li

#pragma warning (disable : 4996)


#include "ValenceViewer.hh"
#include <vector>
#include <float.h>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <limits.h>

//== IMPLEMENTATION ========================================================== 


ValenceViewer::
ValenceViewer(const char* _title, int _width, int _height)
  : MeshViewer(_title, _width, _height)
{ 
  mesh_.request_vertex_colors();

  add_draw_mode("Vertex Valences");

  mesh_.add_property(vertexValence);  

  InitializeColorsArray();
}


//-----------------------------------------------------------------------------


ValenceViewer::
~ValenceViewer()
{
}

//-----------------------------------------------------------------------------

bool
ValenceViewer::
open_mesh(const char* _filename)
{
  // load mesh
  if (MeshViewer::open_mesh(_filename))
  {
    // compute vertex valence and color coding
    calc_valences();
    color_coding();

    glutPostRedisplay();
    return true;
  }
  return false;
}


//-----------------------------------------------------------------------------


void 
ValenceViewer::
calc_valences()
{
  // EXERCISE 1.2 /////////////////////////////////////////////////////////////
  // Compute valence of every vertex of "mesh_" and store them in each vertex
  // using for example custom attributes via dynamic customization
  // (hint: use the Mesh::VertexIter iterator)	
	for (Mesh::VertexIter vIt = mesh_.vertices_begin();
		vIt != mesh_.vertices_end(); ++vIt)
	{
		int valence = 0;
		// Calculate number of neighbors around current vertex
		for (Mesh::VertexVertexIter vvIt = mesh_.vv_iter(*vIt);vvIt; ++vvIt, ++valence);
		mesh_.property(vertexValence, *vIt) = valence;
	}
  /////////////////////////////////////////////////////////////////////////////
}

//-----------------------------------------------------------------------------
void 
ValenceViewer::
InitializeColorsArray()
{
  vertexColors[0] = Mesh::Color(255,0,255);		// Violet
  vertexColors[1] = Mesh::Color(255,255,255);	// White
  vertexColors[2] = Mesh::Color(255,0,0);		// Red
  vertexColors[3] = Mesh::Color(255,125,0);		// Orange
  vertexColors[4] = Mesh::Color(255,255,0);		// Yellow
  vertexColors[5] = Mesh::Color(0,255,0);		// Green
  vertexColors[6] = Mesh::Color(0,255,255);		// Cyan
  vertexColors[7] = Mesh::Color(0,0,255);		// Blue
  vertexColors[8] = Mesh::Color(125,0,125);		// Indigo
  vertexColors[9] = Mesh::Color(125,0,0);		// Maroon
  /////////////////////////////////////////////////////////////////////////////
}


//-----------------------------------------------------------------------------

void 
ValenceViewer::
color_coding()
{
  // EXERCISE 1.3 /////////////////////////////////////////////////////////////
  // Implement a color visualization of your choice that shows the valence of
  // veach ertex of "mesh_". 
  // (hint: use Mesh::Color color type)
	for (Mesh::VertexIter vIt = mesh_.vertices_begin();
		vIt != mesh_.vertices_end(); ++vIt)
	{
		int valence = mesh_.property(vertexValence, *vIt);
		int colorIndex = ((valence < NUMBER_OF_COLORS) ? valence : NUMBER_OF_COLORS) - 1;
		Mesh::Color vColor = vertexColors[colorIndex];
		
		mesh_.set_color(*vIt, vColor);
	}
  /////////////////////////////////////////////////////////////////////////////
}


//-----------------------------------------------------------------------------


void 
ValenceViewer::
draw(const std::string& _draw_mode)
{

  if (indices_.empty())
  {
    MeshViewer::draw(_draw_mode);
    return;
  }

  if (_draw_mode == "Vertex Valences")
  {

	  glDisable(GL_LIGHTING);
	  glShadeModel(GL_SMOOTH);
	  glEnableClientState(GL_VERTEX_ARRAY);
	  glEnableClientState(GL_NORMAL_ARRAY);
	  glEnableClientState(GL_COLOR_ARRAY);
	  GL::glVertexPointer(mesh_.points());
	  GL::glNormalPointer(mesh_.vertex_normals());
	  GL::glColorPointer(mesh_.vertex_colors());
	  glDepthRange(0.01, 1.0);
	  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, &indices_[0]);
	  glDisableClientState(GL_VERTEX_ARRAY);
	  glDisableClientState(GL_NORMAL_ARRAY);
	  glDisableClientState(GL_COLOR_ARRAY);
	  glColor3f(0.1, 0.1, 0.1);
	  glEnableClientState(GL_VERTEX_ARRAY);
	  GL::glVertexPointer(mesh_.points());
	  glDrawBuffer(GL_BACK);
	  glDepthRange(0.0, 1.0);
	  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	  glDepthFunc(GL_LEQUAL);
	  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, &indices_[0]);
	  glDisableClientState(GL_VERTEX_ARRAY);
	  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	  glDepthFunc(GL_LESS);
  }

  else MeshViewer::draw(_draw_mode);
}


//=============================================================================
