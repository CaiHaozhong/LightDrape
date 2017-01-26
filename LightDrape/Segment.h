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

	/* �������岻ͬ�Ĳ�λ����ƥ��Ĳ�λ */
	virtual Region_ getMatch(int bodyPart);
	
	/* �������岻ͬ�Ĳ�λ����ƥ����㷨 */
	virtual RegionFitter_ getRegionFitter(int bodyPart);
	
	/* ��Region����������е�vector�� */
	void addRegionRaw(int type, Region_ region);

	std::vector< std::pair<int, Region_> >& getRegionsRaw();
};
S_PTR(Segment);