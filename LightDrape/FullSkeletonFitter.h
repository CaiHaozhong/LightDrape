#pragma once
#include "simpleskeletonfitter.h"
class Mesh;
S_PTR(Mesh);
class Skeleton;
S_PTR(Skeleton);
class RegionSkeletonizer{
public:
	Skeleton_ skeletonize(Region_ region);

private:
	
};
/* 对两个Region各自进行补洞，骨骼提取，再fit */
class FullSkeletonFitter :
	public SimpleSkeletonFitter
{
public:
	FullSkeletonFitter(void);
	FullSkeletonFitter(Region_ garmentRegion);
	~FullSkeletonFitter(void);

	VertexAlter_ fit(Region_ humanRegion);
	std::string getName();

private:
	Mesh_ asMesh(Region_ region);
};