#include "Segment.h"
#include "RegionFitter.h"

Segment::Segment(void)
{
}


Segment::~Segment(void)
{
}

Region_ Segment::getMatch( int bodyPart )
{
	PRINTLN("In human segment, human part should match cloth part instead of human part");
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

RegionFitter_ Segment::getRegionFitter( int bodyPart )
{
	PRINTLN("In human segment, no algorithm for fitting human part to human part");
	return nullptr;
}
