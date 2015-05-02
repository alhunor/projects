#ifndef VALENCEVIEWERWIDGET_HH
#define VALENCEVIEWERWIDGET_HH


#include "MeshViewer.hh"
#include "ImplicitRBF.hh"
#include "RBFEvaluator.h"

//== DEFINES AND CONSTANTS ====================================================

#define NUMBER_OF_COLORS 10


class ValenceViewer : public MeshViewer
{
public:

	/// default constructor
	ValenceViewer(const char* _title, int _width, int _height);

	// destructor
	~ValenceViewer();

	/// open mesh
	virtual bool open_mesh(const char* _filename);
	void MeshFromFunction(Implicit* ImpFunc);
	virtual void keyboard(int key, int x, int y);


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

	// ReconViewer
	std::vector<Mesh::Point>   Points, Normals;
	float epsilon;
	float beta;
	RBFEvaluator rbfEvaluator;
};

#endif // VALENCEVIEWERWIDGET_HH defined