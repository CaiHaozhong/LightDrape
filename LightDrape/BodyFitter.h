#pragma once
#include "regionfitter.h"
class BodyFitter :
	public RegionFitter
{
public:
	BodyFitter(void);
	BodyFitter(Region_ garmentRegion);
	~BodyFitter(void);

	VertexAlter_ fit(Region_ humanRegion);

	virtual std::string getName();

private:
	void computeAverageXZ(Region_ region, double& x, double& z);
};

