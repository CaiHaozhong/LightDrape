#pragma once
#include "Segment.h"
class ClothSegment :
	public Segment
{	
public:
	const static int CLOTH_TORSO = 0;
	const static int CLOTH_LEFT_SLEEVE = 1;
	const static int CLOTH_RIGHT_SLEEVE = 2;
	const static int CLOTH_PART_COUNT = 3;
private:
	size_t mBodyClothMap[BODY_PART_COUNT];
	Region_ mRegions[CLOTH_PART_COUNT];
public:
	ClothSegment(void);
	~ClothSegment(void);
	Region_ getMatch(int bodyPart);
	void addRegion(int part, Region_ region);
};
S_PTR(ClothSegment);