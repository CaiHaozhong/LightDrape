#pragma once
#include "meshdeformer.h"
class LaplacianMeshDeformer :
	public MeshDeformer
{
public:
	LaplacianMeshDeformer(void);
	~LaplacianMeshDeformer(void);
	void deformMesh(Mesh_ mesh);
};

