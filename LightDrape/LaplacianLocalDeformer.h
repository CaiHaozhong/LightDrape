#pragma once
#include "MeshDeformer.h"
/* 将有位移的顶点与模型分割开，只变形这些顶点 */
class LaplacianLocalDeformer :
	public MeshDeformer
{
public:
	LaplacianLocalDeformer(void);
	~LaplacianLocalDeformer(void);
	void deformMesh(Mesh_ mesh, VertexAlter_ vertexAlter);
};
S_PTR(LaplacianLocalDeformer);

