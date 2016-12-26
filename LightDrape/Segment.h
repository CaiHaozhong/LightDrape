#pragma once
#include "Common.h"
#include "Region.h"
class Segment
{	
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

	/* �������岻ͬ�Ĳ�λ����ƥ��Ĳ�λ */
	virtual Region_ getMatch(BodyPart bodyPart){
		return nullptr;
	}
};
S_PTR(Segment);