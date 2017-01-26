#pragma once
#include "Regionfitter.h"
class LevelSetSkeletonFitter :
	public RegionFitter
{
private:
	Region_ mHumanRegion;
public:
	LevelSetSkeletonFitter(void);
	~LevelSetSkeletonFitter(void);
	LevelSetSkeletonFitter(Region_ garmentRegion);
	VertexAlter_ fit(Region_ humanRegion);
	std::string getName();
};
S_PTR(LevelSetSkeletonFitter);

