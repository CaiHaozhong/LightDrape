#pragma once
#include "Common.h"
class Mesh;
S_PTR(Mesh);
class VertexAlter;
S_PTR(VertexAlter);
class MeshDeformer
{
public:
	MeshDeformer(void);
	~MeshDeformer(void);
	virtual void deformMesh(Mesh_ mesh, VertexAlter_ vertexAlter) = 0;
};
S_PTR(MeshDeformer);