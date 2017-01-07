#include "ClothSegment.h"


ClothSegment::~ClothSegment(void)
{
}

ClothSegment::ClothSegment( void )
{
	for(size_t i = 0; i < BODY_PART_COUNT; i++)
		mBodyClothMap[i] = -1;
	for(size_t i = 0; i < CLOTH_PART_COUNT; i++)
		mRegions[i] = nullptr;
	mBodyClothMap[BODY_LEFT_HAND] = CLOTH_LEFT_SLEEVE;
	mBodyClothMap[BODY_RIGHT_HAND] = CLOTH_RIGHT_SLEEVE;
	mBodyClothMap[BODY_TORSE] = CLOTH_TORSO;
}

Region_ ClothSegment::getMatch( int bodyPart )
{
	size_t clothPart = mBodyClothMap[bodyPart];
	if(clothPart != -1 && clothPart < CLOTH_PART_COUNT)
		return mRegions[clothPart];
	return nullptr;
}

void ClothSegment::addRegion( int part, Region_ region )
{
	if(part < CLOTH_PART_COUNT && part >=0 )
		mRegions[part] = region;
	addRegionRaw(part, region);
}
