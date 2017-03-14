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

	/* 在人体上穿着的高度，Garment的getAlignPoint()的y位置移动到该高度  */
	double dressHeight(Human_ human);
};
S_PTR(Cloth);