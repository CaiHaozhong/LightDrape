#include "LevelSetSkeletonFitter.h"


LevelSetSkeletonFitter::LevelSetSkeletonFitter(void)
{
}

LevelSetSkeletonFitter::LevelSetSkeletonFitter( Region_ garmentRegion ):RegionFitter(garmentRegion){}


LevelSetSkeletonFitter::~LevelSetSkeletonFitter(void)
{
}

VertexAlter_ LevelSetSkeletonFitter::fit( Region_ humanRegion )
{
	mHumanRegion = humanRegion;
}

std::string LevelSetSkeletonFitter::getName()
{
	return "LevelSetSkeletonFitter";
}
