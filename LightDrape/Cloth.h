#pragma once
#include "Garment.h"
class Cloth :
	public Garment
{
public:
	~Cloth(void);
	Cloth(Mesh_ mesh);
	size_t getGeodesicSource();

	Vec3d getAlignPoint();
};
S_PTR(Cloth);