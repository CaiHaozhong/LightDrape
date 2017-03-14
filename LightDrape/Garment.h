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

	/* 返回穿在人体身上的高度，Garment的getAlignPoint()的y位置移动到该高度 */
	virtual double dressHeight(Human_ human) = 0;
};
S_PTR(Garment);