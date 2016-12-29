#pragma once
#include "Common.h"
#include "Region.h"
class Segment
{	
protected:
	std::vector< std::pair<size_t, Region_> > mRegions;
public:
	enum BodyPart{
		BODY_LEFT_HAND = 0,
		BODY_RIGHT_HAND,
		BODY_LEFT_LEG,
		BODY_RIGHT_LEG,
		BODY_HEAD,
		BODY_TORSE,
		BODY_PART_COUNT
	};
	Segment(void);
	~Segment(void);

	/* 根据人体不同的部位返回匹配的部位 */
	virtual Region_ getMatch(BodyPart bodyPart){
		return nullptr;
	}

	/* 将Region添加至父类中的vector中 */
	void addRegionRaw(size_t type, Region_ region){
		mRegions.push_back(std::make_pair(type, region));
	}

	std::vector< std::pair<size_t, Region_> >& getRegionsRaw(){
		return mRegions;
	}
};
S_PTR(Segment);