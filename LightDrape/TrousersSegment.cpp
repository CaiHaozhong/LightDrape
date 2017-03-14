#include "TrousersSegment.h"
#include "FullSkeletonFitter.h"
#include "BodyFitter.h"


TrousersSegment::~TrousersSegment(void)
{
}

TrousersSegment::TrousersSegment( void )
{
	for(size_t i = 0; i < BODY_PART_COUNT; i++)
		mBodyTrousersMap[i] = -1;
	for(size_t i = 0; i < TROUSERS_PART_COUNT; i++)
		mRegions[i] = nullptr;
	mBodyTrousersMap[BODY_LEFT_LEG] = TROUSERS_LEFT;
	mBodyTrousersMap[BODY_RIGHT_LEG] = TROUSERS_RIGHT;
	mBodyTrousersMap[BODY_TORSE] = TROUSERS_WAIST;
}

Region_ TrousersSegment::getMatch( int bodyPart )
{
	if(bodyPart >= 0 && bodyPart < BODY_PART_COUNT){
		size_t trousersPart = mBodyTrousersMap[bodyPart];
		if(trousersPart != -1 && trousersPart < TROUSERS_PART_COUNT && trousersPart >= 0)
			return mRegions[trousersPart];
	}
	return nullptr;
}

void TrousersSegment::addRegion( int part, Region_ region )
{
	if(part < TROUSERS_PART_COUNT && part >= 0)
		mRegions[part] = region;
	addRegionRaw(part, region);
}

RegionFitter_ TrousersSegment::getRegionFitter( int bodyPart )
{
	if(bodyPart == Segment::BODY_LEFT_LEG || bodyPart == Segment::BODY_RIGHT_LEG){
		return std::make_shared<FullSkeletonFitter>(getMatch(bodyPart));
	}
	else if(bodyPart == Segment::BODY_TORSE){
		return std::make_shared<BodyFitter>(getMatch(bodyPart));
	}
	return nullptr;
}
