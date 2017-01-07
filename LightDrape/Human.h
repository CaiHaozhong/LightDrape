#pragma once
#include "Common.h"
#include "Watertightmesh.h"
#include "Garment.h"
class Human :
	public WatertightMesh, public std::enable_shared_from_this<Human>
{
public:	
	~Human(void);
	Human(Mesh_ mesh);

	size_t getGeodesicSource();

	Vec3d getAlignPoint();
	
	void dress(Garment_ garment);
};
S_PTR(Human);