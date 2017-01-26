#pragma once
#include "Meshdeformer.h"
class LaplacianGlobalDeformer :
	public MeshDeformer
{
public:
	LaplacianGlobalDeformer(void);
	~LaplacianGlobalDeformer(void);
	void deformMesh(Mesh_ mesh, VertexAlter_ vertexAlter);
};
S_PTR(LaplacianGlobalDeformer);