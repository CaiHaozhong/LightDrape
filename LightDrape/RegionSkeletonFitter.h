#pragma once
#include "regionfitter.h"
class Quaternion;
S_PTR(Quaternion);
class RegionSkeleton;
S_PTR(RegionSkeleton);
class WatertightMesh;
S_PTR(WatertightMesh);
class RegionSkeletonFitter :
	public RegionFitter
{
private:
	Region_ mHumanRegion;
public:
	RegionSkeletonFitter(void);
	RegionSkeletonFitter(Region_ garmentRegion);
	~RegionSkeletonFitter(void);


	/* 匹配两个Region，并返回服装Region的顶点位移 
	 * 形象的理解：将衣服的一个Region穿到人体的一个Region上
	 * 参数：人体的一个Region */
	VertexAlter_ fit(Region_ humanRegion);

	std::string getName();

	void fitSkeleton(std::vector< std::pair<size_t, Vec3d> >& garSkeTrans, 
		std::vector< std::pair<size_t, std::pair<Vec3d, double> > >& garSkeRotate);
private:
	/* 加权计算骨骼节点对应的网格顶点的位移
	 * 每个网格顶点受其对应的骨骼节点及相邻两个骨骼节点的影响，边界则为一个
	 * 按网格顶点与骨骼节点的距离加权平均
	 */
	void computeTranslation(VertexAlter_ ret,
		std::vector< std::pair<size_t, Vec3d> >& garSkeTrans);

	/* 对骨骼节点对应的网格顶点进行旋转 */
	void computeRotation(VertexAlter_ ret,
		std::vector< std::pair<size_t, std::pair<Vec3d, double> > >& garSkeRotate);
};

