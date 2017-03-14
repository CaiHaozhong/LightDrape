#pragma once
#include "garment.h"
class Trousers :
	public Garment
{
public:
	Trousers(Mesh_ mesh);
	~Trousers(void);
	size_t getGeodesicSource();

	Vec3d getAlignPoint();

	/* 在人体上穿着的高度，Garment的getAlignPoint()的y位置移动到该高度  */
	double dressHeight(Human_ human);
};

