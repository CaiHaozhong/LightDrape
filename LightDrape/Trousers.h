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

	/* �������ϴ��ŵĸ߶ȣ�Garment��getAlignPoint()��yλ���ƶ����ø߶�  */
	double dressHeight(Human_ human);
};

