#pragma once
#include "MeshDeformer.h"
/* ����λ�ƵĶ�����ģ�ͷָ��ֻ������Щ���� */
class LaplacianLocalDeformer :
	public MeshDeformer
{
public:
	LaplacianLocalDeformer(void);
	~LaplacianLocalDeformer(void);
	void deformMesh(Mesh_ mesh, VertexAlter_ vertexAlter);
};
S_PTR(LaplacianLocalDeformer);

