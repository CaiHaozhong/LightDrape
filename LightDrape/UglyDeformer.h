#pragma once
#include "Common.h"
#include "meshdeformer.h"
class UglyDeformer :
	public MeshDeformer
{
public:
	UglyDeformer(void);
	~UglyDeformer(void);

	void deformMesh(Mesh_ mesh);
};
S_PTR(UglyDeformer);
