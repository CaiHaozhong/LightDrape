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
	
	/* ��������Ĳ�ͬ��λ���õ���Ӧ�Ŀ��ӵĲ�λ */
	Region_ getMatch(int bodyPart);

	/* ���ݿ��ӵĲ�ͬ��λ������� */
	void addRegion(int part, Region_ region);


	/* �������岻ͬ�Ĳ�λ����ƥ��ķ��� */
	RegionFitter_ getRegionFitter(int bodyPart);
private:
	size_t mBodyTrousersMap[BODY_PART_COUNT];
	Region_ mRegions[TROUSERS_PART_COUNT];
};
S_PTR(TrousersSegment);