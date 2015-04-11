#pragma warning (disable : 4996)
  
int tutorial1()
{
 
 PolyMesh mesh;
  // generate vertices
  PolyMesh::VertexHandle vhandle[8];
  vhandle[0] = mesh.add_vertex(PolyMesh::Point(-1, -1,  1));
  vhandle[1] = mesh.add_vertex(PolyMesh::Point( 1, -1,  1));
  vhandle[2] = mesh.add_vertex(PolyMesh::Point( 1,  1,  1));
  vhandle[3] = mesh.add_vertex(PolyMesh::Point(-1,  1,  1));
  vhandle[4] = mesh.add_vertex(PolyMesh::Point(-1, -1, -1));
  vhandle[5] = mesh.add_vertex(PolyMesh::Point( 1, -1, -1));
  vhandle[6] = mesh.add_vertex(PolyMesh::Point( 1,  1, -1));
  vhandle[7] = mesh.add_vertex(PolyMesh::Point(-1,  1, -1));
  // generate (quadrilateral) faces
  std::vector<PolyMesh::VertexHandle>  face_vhandles;
  
  face_vhandles.clear();
  face_vhandles.push_back(vhandle[0]);
  face_vhandles.push_back(vhandle[1]);
  face_vhandles.push_back(vhandle[2]);
  face_vhandles.push_back(vhandle[3]);
  mesh.add_face(face_vhandles);
 
  face_vhandles.clear();
  face_vhandles.push_back(vhandle[7]);
  face_vhandles.push_back(vhandle[6]);
  face_vhandles.push_back(vhandle[5]);
  face_vhandles.push_back(vhandle[4]);
  mesh.add_face(face_vhandles);

  face_vhandles.clear();
  face_vhandles.push_back(vhandle[1]);
  face_vhandles.push_back(vhandle[0]);
  face_vhandles.push_back(vhandle[4]);
  face_vhandles.push_back(vhandle[5]);
  mesh.add_face(face_vhandles);
  
  face_vhandles.clear();
  face_vhandles.push_back(vhandle[2]);
  face_vhandles.push_back(vhandle[1]);
  face_vhandles.push_back(vhandle[5]);
  face_vhandles.push_back(vhandle[6]);
  mesh.add_face(face_vhandles);
  
  face_vhandles.clear();
  face_vhandles.push_back(vhandle[3]);
  face_vhandles.push_back(vhandle[2]);
  face_vhandles.push_back(vhandle[6]);
  face_vhandles.push_back(vhandle[7]);
  mesh.add_face(face_vhandles);
  
  face_vhandles.clear();
  face_vhandles.push_back(vhandle[0]);
  face_vhandles.push_back(vhandle[3]);
  face_vhandles.push_back(vhandle[7]);
  face_vhandles.push_back(vhandle[4]);
  mesh.add_face(face_vhandles);
  // write mesh to output.obj
  try
  {
    if ( !OpenMesh::IO::write_mesh(mesh, "output.off") )
    {
      std::cerr << "Cannot write mesh to file 'output.off'" << std::endl;
      return 1;
    }
  }
  catch( std::exception& x )
  {
    std::cerr << x.what() << std::endl;
    return 1;
  }
  return 0;
} // void tutorial1()

int tutorial1Hu()
{
 
 TriMesh mesh;
 // generate vertices
 TriMesh::VertexHandle vhandle[4];

 vhandle[0] = mesh.add_vertex(TriMesh::Point(-1, 1,  0));
 vhandle[1] = mesh.add_vertex(TriMesh::Point( 1, 1,  0));
 vhandle[2] = mesh.add_vertex(TriMesh::Point( 0.5,  0.5,  8));
 vhandle[3] = mesh.add_vertex(TriMesh::Point(0, -1,  0));
 // generate (quadrilateral) faces
 std::vector<TriMesh::VertexHandle>  face_vhandles;

 face_vhandles.clear();
 face_vhandles.push_back(vhandle[0]);
 face_vhandles.push_back(vhandle[2]);
 face_vhandles.push_back(vhandle[1]);
 mesh.add_face(face_vhandles);


 face_vhandles.clear();
 face_vhandles.push_back(vhandle[1]);
 face_vhandles.push_back(vhandle[2]);
 face_vhandles.push_back(vhandle[3]);
 mesh.add_face(face_vhandles);



 face_vhandles.clear();
 face_vhandles.push_back(vhandle[3]);
 face_vhandles.push_back(vhandle[2]);
 face_vhandles.push_back(vhandle[0]);
 mesh.add_face(face_vhandles);


 face_vhandles.clear();
 face_vhandles.push_back(vhandle[0]);
 face_vhandles.push_back(vhandle[1]);
 face_vhandles.push_back(vhandle[3]);
 mesh.add_face(face_vhandles);

 // write mesh to output.obj
 try
  {
    if ( !OpenMesh::IO::write_mesh(mesh, "tetrahedron.off") )
    {
      std::cerr << "Cannot write mesh to file 'output.off'" << std::endl;
      return 1;
    }
  }
  catch( std::exception& x )
  {
    std::cerr << x.what() << std::endl;
    return 1;
  }
  return 0;
} // int tutorial1Hu()


int tutorial2(char* filenameIn, char* filenameOut, unsigned int iterations)
{
 TriMesh  mesh;

 // read mesh from stdin
 if ( ! OpenMesh::IO::read_mesh(mesh, filenameIn) )
 {
  std::cerr << "Error: Cannot read mesh from " << filenameIn << std::endl;
  return 1;
 }


 // this vector stores the computed centers of gravity
 std::vector<TriMesh::Point>  cogs;
 std::vector<TriMesh::Point>::iterator cog_it;
 cogs.reserve(mesh.n_vertices());


 // smoothing mesh "iterations" times
 TriMesh::VertexIter          v_it, v_end(mesh.vertices_end());
 TriMesh::VertexVertexIter    vv_it;
 TriMesh::Point               cog;
 TriMesh::Scalar              valence;
 unsigned int                i;

 for (i=0; i < iterations; ++i)
 {
  cogs.clear();
  for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
  {
   cog = TriMesh::Point(0,  0,  0);
   valence = 0.0;

   for (vv_it=mesh.vv_iter( v_it ); vv_it; ++vv_it)
   {
    cog += mesh.point( vv_it );
    ++valence;
   }

   cogs.push_back(cog / valence);
  } // for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)

  for (v_it=mesh.vertices_begin(), cog_it=cogs.begin(); v_it!=v_end; ++v_it, ++cog_it)
  {
   if ( !mesh.is_boundary( v_it ) )
   {
    mesh.set_point( v_it, *cog_it );
   }
  }
 } // for (i=0; i < iterations; ++i)

 // write mesh to stdout
 if ( ! OpenMesh::IO::write_mesh(mesh, filenameOut) )
 {
  std::cerr << "Error: cannot write mesh to " << filenameOut << std::endl;
  return 1;
 }
 return 0;
} // tutorial 2


TriMesh createTetraOld(double edgelen)
{
	TriMesh mesh;
	TriMesh::VertexHandle vhandle[4];
	vhandle[0] = mesh.add_vertex(PolyMesh::Point(edgelen/2, 0,  -edgelen*sqrt(2.0)/4.0));
	vhandle[1] = mesh.add_vertex(PolyMesh::Point(-edgelen/2, 0,  -edgelen*sqrt(2.0)/4.0));
	vhandle[2] = mesh.add_vertex(PolyMesh::Point( 0,  -edgelen/2,  edgelen*sqrt(2.0)/4.0));
	vhandle[3] = mesh.add_vertex(PolyMesh::Point(0,  edgelen/2,  edgelen*sqrt(2.0)/4.0));

	std::vector<TriMesh::VertexHandle>  face_vhandles;

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[0]);
	mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[3]);
	mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[1]);
	mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[3]);
	mesh.add_face(face_vhandles);

	return mesh;
}// TriMesh createTetraOld()