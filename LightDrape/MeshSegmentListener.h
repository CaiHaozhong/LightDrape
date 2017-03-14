#pragma once
#include "Common.h"
class Segment;
S_PTR(Segment);
class MeshSegmentListener
{
public:
	MeshSegmentListener(void);
	~MeshSegmentListener(void);
	virtual void onEndCoarseSegment(Segment_ seg) = 0;
};

