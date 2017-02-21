#pragma once
#include "lefttorserightrefiner.h"
#include <vector>
class Region;
S_PTR(Region);
class MeshSegmenter;
S_PTR(MeshSegmenter);
class LeftTorseRightSimpleRefiner :
	public LeftTorseRightRefiner
{
public:
	LeftTorseRightSimpleRefiner(void);
	~LeftTorseRightSimpleRefiner(void);
	LeftTorseRightSimpleRefiner(MeshSegmenter_ meshSegmenter, Region_ left, Region_ torse, Region_ right);
	void refine();
private:
	void computeBoundedXZ(Region_ region,
		double& minX, double& maxX,
		double& minZ, double& maxZ,
		double& minY);
	void refineRegion(Region_ region);
};

