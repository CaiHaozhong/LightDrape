#pragma once
#include "Watertightmesh.h"
class Garment :
	public WatertightMesh
{
public:
	Garment(Mesh_ mesh);
	~Garment(void);
};
S_PTR(Garment);