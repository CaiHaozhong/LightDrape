#pragma once
#include "Common.h"
#include "Watertightmesh.h"
class Human :
	public WatertightMesh
{
public:
	~Human(void);
	size_t getGeodesicSource();
};
S_PTR(Human);