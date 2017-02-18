#pragma once
#include "regionfitter.h"
class Quaternion;
S_PTR(Quaternion);
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
		std::vector< std::pair<size_t, Quaternion_ > >& garSkeRotate);
};

