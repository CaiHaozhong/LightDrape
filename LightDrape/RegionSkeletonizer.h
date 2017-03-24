#pragma once
#include "Common.h"
#include <string>
class Skeleton;
S_PTR(Skeleton);
class Region;
S_PTR(Region);
class RegionSkeletonizer{
public:
	Skeleton_ skeletonize(Region_ region, std::string name = "");
private:
	std::string getSkeletonFileName( std::string name );

	bool hasSkeletonCached( std::string name);

	Skeleton_ readSkeleton( std::string file );

	bool cachSkeleton( Skeleton_ skeleton, std::string file );

};
S_PTR(RegionSkeletonizer);

