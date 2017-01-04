#pragma once
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


};

