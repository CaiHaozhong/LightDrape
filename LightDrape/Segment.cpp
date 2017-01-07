#include "Segment.h"


Segment::Segment(void)
{
}


Segment::~Segment(void)
{
}

Region_ Segment::getMatch( int bodyPart )
{
	return nullptr;
}

void Segment::addRegionRaw( int type, Region_ region )
{
	mRegions.push_back(std::make_pair(type, region));
}

std::vector< std::pair<int, Region_> >& Segment::getRegionsRaw()
{
	return mRegions;
}
