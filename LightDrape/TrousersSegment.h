#pragma once
#include "segment.h"
class TrousersSegment :
	public Segment
{
public:
	const static int TROUSERS_WAIST = 0;
	const static int TROUSERS_LEFT = 1;
	const static int TROUSERS_RIGHT = 2;
	const static int TROUSERS_PART_COUNT = 3;
	TrousersSegment(void);
	~TrousersSegment(void);
	
	/* 根据人体的不同部位来得到对应的裤子的部位 */
	Region_ getMatch(int bodyPart);

	/* 根据裤子的不同部位添加区域 */
	void addRegion(int part, Region_ region);


	/* 根据人体不同的部位返回匹配的方法 */
	RegionFitter_ getRegionFitter(int bodyPart);
private:
	size_t mBodyTrousersMap[BODY_PART_COUNT];
	Region_ mRegions[TROUSERS_PART_COUNT];
};
S_PTR(TrousersSegment);