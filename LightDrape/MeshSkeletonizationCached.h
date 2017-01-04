#pragma once
#include "MyPrecompileHeader.h"
class MeshSkeletonizationCached :
	public MeshSkeletonization
{
public:
	MeshSkeletonizationCached(void);
	~MeshSkeletonizationCached(void);
	void skeletonize(WatertightMesh_ mesh);
private:
	std::string getSkeletonFileName(WatertightMesh_ mesh);
	bool hasSkeletonCached(WatertightMesh_ mesh);

	Skeleton_ readSkeleton(std::string file);

	bool cachSkeleton(Skeleton_ skeleton, std::string file);
};
S_PTR(MeshSkeletonizationCached);