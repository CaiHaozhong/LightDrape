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

	/* �������ϴ��ŵĸ߶ȣ�Garment��getAlignPoint()��yλ���ƶ����ø߶�  */
	double dressHeight(Human_ human);
};
S_PTR(Cloth);