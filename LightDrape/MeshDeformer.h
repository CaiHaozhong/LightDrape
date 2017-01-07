#pragma once
#include "Common.h"
class Mesh;
S_PTR(Mesh);
class MeshDeformer
{
public:
	MeshDeformer(void);
	~MeshDeformer(void);
	virtual void deformMesh(Mesh_ mesh) = 0;
};
S_PTR(MeshDeformer);