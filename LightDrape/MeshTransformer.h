#pragma once
#include "Common.h"
#include "Mesh.h"
class MeshTransformer
{
protected:
	Mesh_ mMesh;
public:
	MeshTransformer(void);
	~MeshTransformer(void);
	MeshTransformer(Mesh_ mesh){
		mMesh = mesh;
	}

	void transform();
};
S_PTR(MeshTransformer);