#include "BodyFitter.h"
#include "Region.h"
#include "Vec3d.h"
#include "VertexAlter.h"


BodyFitter::BodyFitter(void)
{
}

BodyFitter::BodyFitter( Region_ garmentRegion ) :RegionFitter(garmentRegion)
{

}


BodyFitter::~BodyFitter(void)
{
}

VertexAlter_ BodyFitter::fit( Region_ humanRegion )
{
	VertexAlter_ ret = smartNew(VertexAlter);
	double gx, gz, hx, hz;
	computeAverageXZ(humanRegion, hx, hz);
	computeAverageXZ(mGarmentRegion, gx, gz);
	auto& vers = mGarmentRegion->getVertices();
	Vec3d delta(hx-gx, 0, hz-gz);
	for(auto it = vers.begin(); it != vers.end(); it++){
		ret->add(*it, delta);
	}
	return ret;
}

std::string BodyFitter::getName()
{
	return "BodyFitter";
}

void BodyFitter::computeAverageXZ( Region_ region, double& x, double& z )
{
	x = 0;
	z = 0;
	RegionSkeleton_ regionSke = region->getRegionSkeleton();
	for(size_t i = 0; i < regionSke->count(); i++){
		Vec3d& center = regionSke->getNode(i)->center;
		x += center.values_[0];
		z += center.values_[2];
	}
	x /= regionSke->count();
	z /= regionSke->count();
}
