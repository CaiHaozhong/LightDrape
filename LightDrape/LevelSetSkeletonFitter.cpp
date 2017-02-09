#include "LevelSetSkeletonFitter.h"
#include "LevelSet.h"
#include <vector>

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
	std::vector<LevelCircle_> humanCircles;
	std::vector<LevelCircle_> garmetCircles;
	std::vector<double> delta;
	size_t garCircleSize = garmetCircles.size();
	size_t humanCircleSize = humanCircles.size();
	delta.reserve(garCircleSize);
	for(size_t i = 0; i < garCircleSize; i++){
		if(i >= humanCircleSize){
			// 比躯干长的部分保持不动
			break;
		}
	}
}

std::string LevelSetSkeletonFitter::getName()
{
	return "LevelSetSkeletonFitter";
}
