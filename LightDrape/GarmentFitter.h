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
public:
	GarmentFitter(void);
	~GarmentFitter(void);
	GarmentFitter(Garment_ garment);

	void setGarment(Garment_ garment);

	/* 设置变形的算法 */
	void setMeshDeformer(MeshDeformer_ meshDeformer);

	/* 将衣服穿到人身上, 这将变形衣服 */
	void fit(Human_ human);

private:

	/* 将衣服移动到人体身上 */
	void translateGarment();

// 	/* 根据骨骼节点的位移来将对应的网格顶点的位移记录在模型中 */
// 	void alterGarment();
	
};
S_PTR(GarmentFitter);