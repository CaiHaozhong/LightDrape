#include "RegionFitter.h"
#include "Region.h"

RegionFitter::RegionFitter(void)
{
}


RegionFitter::~RegionFitter(void)
{
}

RegionFitter::RegionFitter(Region_ garmentRegion){
	this->mGarmentRegion = garmentRegion;
}

void RegionFitter::setGarmentRegion(Region_ garmentRegion){
	this->mGarmentRegion = garmentRegion;
}