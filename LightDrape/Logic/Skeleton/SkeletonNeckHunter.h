#pragma once
#include "Skeleton.h"
class SkeletonCoincidentPointHunter
{
public:
	/** 返回衣服和人体骨骼待重合的点的下标 **/
	virtual size_t findNeck(const Skeleton skeleton) = 0;
};

