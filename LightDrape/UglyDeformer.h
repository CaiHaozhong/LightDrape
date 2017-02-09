#pragma once
#include "Common.h"
#include "meshdeformer.h"
class VertexAlter;
S_PTR(VertexAlter);
class Mesh;
S_PTR(Mesh);
class UglyDeformer :
	public MeshDeformer
{
public:
	UglyDeformer(void);
	~UglyDeformer(void);

	void deformMesh(Mesh_ mesh, VertexAlter_ vertexAlter);
};
S_PTR(UglyDeformer);
