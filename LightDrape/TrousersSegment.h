#pragma once
#include "segment.h"
class TrousersSegment :
	public Segment
{
public:
	enum TrousersPart{
		TROUSERS_WAIST = 0,
		TROUSERS_LEFT,
		TROUSERS_RIGHT,
		TROUSERS_PART_COUNT
	};
	TrousersSegment(void){
		for(size_t i = 0; i < BODY_PART_COUNT; i++)
			mBodyTrousersMap[i] = -1;
		for(size_t i = 0; i < TROUSERS_PART_COUNT; i++)
			mRegions[i] = nullptr;
		mBodyTrousersMap[BODY_LEFT_LEG] = TROUSERS_LEFT;
		mBodyTrousersMap[BODY_RIGHT_LEG] = TROUSERS_RIGHT;
		mBodyTrousersMap[BODY_TORSE] = TROUSERS_WAIST;
	}
	~TrousersSegment(void);
	
	/* 根据人体的不同部位来得到对应的裤子的部位 */
	Region_ getMatch(BodyPart bodyPart){
		if(bodyPart >= 0 && bodyPart < BODY_PART_COUNT){
			size_t trousersPart = mBodyTrousersMap[bodyPart];
			if(trousersPart != -1 && trousersPart < TROUSERS_PART_COUNT && trousersPart >= 0)
				return mRegions[trousersPart];
		}
		return nullptr;
	}

	/* 根据裤子的不同部位添加区域 */
	void addRegion(TrousersPart part, Region_ region){
		if(part < TROUSERS_PART_COUNT && part >= 0)
			mRegions[part] = region;
	}
private:
	size_t mBodyTrousersMap[BODY_PART_COUNT];
	Region_ mRegions[TROUSERS_PART_COUNT];
};
S_PTR(TrousersSegment);