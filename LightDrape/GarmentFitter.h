#pragma once
#include "Common.h"
#include "Vec3d.h"
#include <vector>
#include <tuple>
class Garment;
S_PTR(Garment);
class Human;
S_PTR(Human);
class Region;
S_PTR(Region);
class MeshDeformer;
S_PTR(MeshDeformer);
class GarmentFitter
{
private:
	Garment_ mGarment;
	Human_ mHuman;
	MeshDeformer_ mMeshDeformer;
	std::vector< std::pair<size_t, Vec3d> > mGarSkeTrans;
public:
	GarmentFitter(void);
	~GarmentFitter(void);
	GarmentFitter(Garment_ garment);

	void setGarment(Garment_ garment);

	void setMeshDeformer(MeshDeformer_ meshDeformer);

	/* ����, �����·� */
	void fit(Human_ human);

private:
	/* ���������һ��Region�������·���һ��Region */
	void fit(Region_ garmentRegion, Region_ humanRegion);

	/* ���·��ƶ����������� */
	void translateGarment();
	
	/* ��¼�����ڵ��λ�� */
	void skeTranslateTo(size_t ske, Vec3d dest);

	/* ���ݹ����ڵ��λ��������Ӧ�����񶥵��λ�Ƽ�¼��ģ���� */
	void alterGarment();
	
};
S_PTR(GarmentFitter);