#ifndef _MESHUTIL_H
#define _MESHUTIL_H

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#define PRECISION double

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>
#include <OpenMesh/Core/Mesh/Status.hh>
#include "point.h"


struct MyTraits : public OpenMesh::DefaultTraits
{
  VertexAttributes(OpenMesh::Attributes::Status);
  FaceAttributes(OpenMesh::Attributes::Status);
  EdgeAttributes(OpenMesh::Attributes::Status);
};

typedef OpenMesh::TriMesh_ArrayKernelT<MyTraits> TriMesh;

void extendMesh(TriMesh& mesh, TriMesh::FaceHandle& fh, OpenMesh::Vec3f& P, bool runGarbageCollector=true);

TriMesh createTetra(MyREAL edgelen);
TriMesh createSphere(MyREAL radius, int subdivisionLevel);
TriMesh createTorus(MyREAL bigR, MyREAL smallr, unsigned int k);

MyREAL angle(OpenMesh::Vec3f& pointA, OpenMesh::Vec3f& pointB, OpenMesh::Vec3f& pointC);


// adds a new face to the mesh, if the verteces are already part of the mesh
TriMesh::FaceHandle addFaceSimple(TriMesh& mesh, TriMesh::VertexHandle a, TriMesh::VertexHandle b, TriMesh::VertexHandle c);


// adds a new face to the mesh, if the verteces are already part of the mesh
void addFace(TriMesh& mesh, TriMesh::VertexHandle a, TriMesh::VertexHandle b, TriMesh::VertexHandle c);

void subdivide(TriMesh& mesh, int k);

#endif