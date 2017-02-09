#pragma once
#include "Regionfitter.h"
#include <vector>
#include "Vec3d.h"
class LevelCircle;
S_PTR(LevelCircle);
class Mesh;
S_PTR(Mesh);
class VertexAlter;
S_PTR(VertexAlter);
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
private:
	VertexAlter_ computeVertexAlter( Mesh_ garMesh, std::vector<Vec3d>& delta, 
		std::vector<LevelCircle_>& circles);
};
S_PTR(LevelSetSkeletonFitter);

