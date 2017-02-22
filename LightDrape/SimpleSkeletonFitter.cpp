#include "SimpleSkeletonFitter.h"
#include "VertexAlter.h"
#include "Region.h"
#include <algorithm>
#include "Skeleton.h"
#include "WatertightMesh.h"


SimpleSkeletonFitter::SimpleSkeletonFitter(void)
{
}

SimpleSkeletonFitter::SimpleSkeletonFitter( Region_ garmentRegion ):SkeletonFitter(garmentRegion){}


SimpleSkeletonFitter::~SimpleSkeletonFitter(void)
{
}

VertexAlter_ SimpleSkeletonFitter::fit( Region_ humanRegion )
{
	return SkeletonFitter::fit(humanRegion);
}

std::string SimpleSkeletonFitter::getName(){
	return "SimpleSkeletonFitter";
}

size_t SimpleSkeletonFitter::getStartSkeletonNode( Region_ region )
{
	return region->getStart();
}

Skeleton_ SimpleSkeletonFitter::getSkeleton( Region_ region )
{
	return region->getSkeleton();
}

std::set<size_t>& SimpleSkeletonFitter::getSkeletonNodesInRegion( Region_ region )
{
	return region->getSkeNodes();
}

