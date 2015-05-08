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
#include <IsoEx/Grids/ScalarGridT.hh>
#include <IsoEx/Extractors/MarchingCubesT.hh>

#include "ImplicitRBF.hh"
#include "ImplicitMLS.hh"
const int MC_RESOLUTION = 50;

#include <fileSystems.h>

//== IMPLEMENTATION ========================================================== 


ValenceViewer::ValenceViewer(const char* _title, int _width, int _height) : MeshViewer(_title, _width, _height)
{
	mesh_.request_vertex_colors();

	add_draw_mode("Vertex Valences");
	mesh_.add_property(vertexValence);  
	InitializeColorsArray();

	// Reconviewer
	epsilon=0.01;
	beta = 0.61;
	add_draw_mode("Point Cloud");
}


//-----------------------------------------------------------------------------


ValenceViewer::~ValenceViewer()
{
	mesh_.clear();
}

//-----------------------------------------------------------------------------

bool ValenceViewer::open_mesh(const char* _filename)
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


void ValenceViewer::calc_valences()
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
void ValenceViewer::InitializeColorsArray()
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
}


//-----------------------------------------------------------------------------

void ValenceViewer::color_coding()
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


void ValenceViewer::draw(const std::string& _draw_mode)
{
	if (_draw_mode == "Point Cloud")
	{
		//drawing point cloud
		glDisable(GL_LIGHTING);
		glPointSize(5.0);
		glBegin(GL_POINTS);
		for (std::vector<Mesh::Point>::iterator pi=Points.begin();pi!=Points.end();pi++)
		{
			glVertex3d((*pi)[0], (*pi)[1],(*pi)[2]);
		}
		glEnd();
		return;
	}

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
	} else
	{
		 MeshViewer::draw(_draw_mode);
	}
} // void ValenceViewer::draw(const std::string& _draw_mode)



void ValenceViewer::MeshFromFunction(Implicit* ImpFunc)
{
	if (Points.empty()) return;
	std::cout << "Bounding Box\n" << std::flush;
	Mesh::Point bb_min( Points[0]), bb_max( Points[0]);

	for (std::vector<Mesh::Point>::iterator pi=Points.begin(); pi!=Points.end();pi++)
	{
		bb_min.minimize(*pi);
		bb_max.maximize(*pi);
	}

	Mesh::Point  bb_center = (bb_max+bb_min)*0.5f;
	OpenMesh::Vec3d VecDiag(bb_max[0]-bb_min[0], bb_max[1]-bb_min[1], bb_max[2]-bb_min[2]);
	bb_min = bb_center - 0.6f * Mesh::Point(VecDiag[0], VecDiag[1], VecDiag[2]);
	bb_max = bb_center + 0.6f * Mesh::Point(VecDiag[0], VecDiag[1], VecDiag[2]);

	// setup Marching Cubes grid by sampling RBF
	std::cout << "Setup grid\n" << std::flush;
	float MeanSize = VecDiag.mean();
	int res[3];
	int idir;
	for (idir=0; idir<3; idir++)
	{
		res[idir] = (int)(MC_RESOLUTION * VecDiag[0]/MeanSize + 0.5); 
	}
	IsoEx::ScalarGridT<Mesh::Scalar>  grid(bb_min,
		Mesh::Point(bb_max[0]-bb_min[0], 0, 0),
		Mesh::Point(0, bb_max[1]-bb_min[1], 0),
		Mesh::Point(0, 0, bb_max[2]-bb_min[2]),
		res[0], res[1], res[2]);

	for (int x=0; x<res[0]; ++x)
		for (int y=0; y<res[1]; ++y)
			for (int z=0; z<res[2]; ++z)
				grid.value(x,y,z) = (*ImpFunc)( grid.point(x,y,z) );


	// isosurface extraction by Marching Cubes
	std::cout << "Marching Cubes\n" << std::flush;
	mesh_.clear();
	marching_cubes(grid, mesh_); 

	mesh_.update_normals();

	// update face indices for faster rendering
	update_face_indices();
    
	// info
	std::cerr << mesh_.n_vertices() << " vertices, " << mesh_.n_faces()    << " faces\n";
} // void ValenceViewer::MeshFromFunction(Implicit* ImpFunc)



void ValenceViewer::keyboard(int key, int x, int y) 
{
  switch (key)
  {
    case 'o':
    {
		//.pts (point cloud) file opening
	    ;
		char FileName [MAX_PATH];

		
		if (ShowOpenFileDialog(FileName, MAX_PATH, "PointSets with normals\0*.ptsn\0All\0*.*\0Text\0*.TXT\0"))
		{
			Points.clear();
			Normals.clear();
			std::ifstream ifs(FileName);
			if (!ifs) 
			{
				std::cerr << "Cannot open file\n";
				exit(1);
			}

			Mesh::Point                p, n;

			while (ifs && !ifs.eof())
			{
				ifs >> p[0] >> p[1] >> p[2];
				ifs >> n[0] >> n[1] >> n[2];
				Points.push_back(p);
				Normals.push_back(n);
			}
			std::cout << Points.size() << " sample points\n";
			ifs.close();

			//establishing bounding box for drawing
			Mesh::Point            bbMin, bbMax;
			bbMin=*(Points.begin());
			bbMax=*(Points.begin());
			for (std::vector<Mesh::Point>::iterator pi=Points.begin();pi!=Points.end();pi++){
				bbMin.minimize(*pi);
				bbMax.maximize(*pi);
			}
			set_scene( (Vec3f)(bbMin + bbMax)*0.5, 0.5*(bbMin - bbMax).norm());
		}
		break;
    }

	
	//RBF interpolation
	case 'r':{
		std::cout << "Fit RBF\n" << std::flush;
		ImplicitRBF  implicitRBF_( Points, Normals, epsilon, &rbfEvaluator );
		MeshFromFunction((Implicit*)&implicitRBF_);
		break;
			 }
	
	case 'm':{
		std::cout << "Fit MLS\n" << std::flush;
		ImplicitMLS  implicitMLS_( Points, Normals);
		MeshFromFunction((Implicit*)&implicitMLS_);
		break;
			 }

	case GLUT_KEY_UP:
		epsilon+=0.01;
		std::cout <<"Epsilon = "<<epsilon<<"\n"<<std::flush;
		break;

	case GLUT_KEY_DOWN:
		if (epsilon>0.015){
			epsilon-=0.01;
			std::cout <<"Epsilon = "<<epsilon<<"\n"<<std::flush;
		}
		break;
	case '[':
		if (rbfEvaluator.Beta>0.015){
			rbfEvaluator.Beta-=0.01;
			std::cout <<"Beta = "<<rbfEvaluator.Beta<<"\n"<<std::flush;
		}
		break;
	case ']':
		rbfEvaluator.Beta+=0.01;
		std::cout <<"Beta = "<< rbfEvaluator.Beta <<"\n"<<std::flush;
		break;

	case 't':
		{
			rbfEvaluator.Next();
			std::cout << "RBF basis functions set to " << rbfEvaluator.ToString() << "\n" << std::flush;
			break;
		}
    default:
    {
      GlutExaminer::keyboard(key, x, y);
      break;
    }
  }
}


//=============================================================================
