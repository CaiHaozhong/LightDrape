#pragma once
#include "Watertightmesh.h"
class Human;
S_PTR(Human);
class Garment :
	public WatertightMesh
{
public:
	Garment(Mesh_ mesh);
	~Garment(void);

	/* ���ش����������ϵĸ߶ȣ�Garment��getAlignPoint()��yλ���ƶ����ø߶� */
	virtual double dressHeight(Human_ human) = 0;
};
S_PTR(Garment);