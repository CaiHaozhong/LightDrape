#pragma once
#include "Common.h"
#include "Region.h"
class RegionFitter;
S_PTR(RegionFitter);
class Segment
{	
protected:
	std::vector< std::pair<int, Region_> > mRegions;
public:
	const static int BODY_LEFT_HAND = 0;	
	const static int BODY_RIGHT_HAND = 1;
	const static int BODY_LEFT_LEG = 2;
	const static int BODY_RIGHT_LEG = 3;
	const static int BODY_HEAD = 4;
	const static int BODY_TORSE = 5;
	const static int BODY_PART_COUNT = 6;

	Segment(void);
	~Segment(void);

	/* 根据人体不同的部位返回匹配的部位 */
	virtual Region_ getMatch(int bodyPart);
	
	/* 根据人体不同的部位返回匹配的算法 */
	virtual RegionFitter_ getRegionFitter(int bodyPart);
	
	/* 将Region添加至父类中的vector中 */
	void addRegionRaw(int type, Region_ region);

	std::vector< std::pair<int, Region_> >& getRegionsRaw();
};
S_PTR(Segment);