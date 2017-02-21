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
/* ������Region���Խ��в�����������ȡ����fit */
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