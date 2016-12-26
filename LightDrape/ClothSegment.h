#pragma once
#include "Segment.h"
#include <map>
class ClothSegment :
	public Segment
{	
public:
	enum ClothPart{
		CLOTH_TORSE = 0,
		CLOTH_LEFT_SLEEVES,
		CLOTH_RIGHT_SLEEVES,
		CLOTH_PART_COUNT
	};
private:
	size_t mBodyClothMap[BODY_PART_COUNT];
	Region_ mRegions[CLOTH_PART_COUNT];
public:
	ClothSegment(void){
		for(size_t i = 0; i < BODY_PART_COUNT; i++)
			mBodyClothMap[i] = -1;
		for(size_t i = 0; i < CLOTH_PART_COUNT; i++)
			mRegions[i] = nullptr;
		mBodyClothMap[BODY_LEFT_HAND] = CLOTH_LEFT_SLEEVES;
		mBodyClothMap[BODY_RIGHT_HAND] = CLOTH_RIGHT_SLEEVES;
		mBodyClothMap[BODY_TORSE] = CLOTH_TORSE;
	}
	~ClothSegment(void);
	Region_ getMatch(BodyPart bodyPart){
		size_t clothPart = mBodyClothMap[bodyPart];
		if(clothPart != -1 && clothPart < CLOTH_PART_COUNT)
			return mRegions[clothPart];
		return nullptr;
	}
	void addRegion(ClothPart part, Region_ region){
		if(part < CLOTH_PART_COUNT && part >=0 )
			mRegions[part] = region;
	}
};
S_PTR(ClothSegment);