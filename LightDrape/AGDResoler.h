#pragma once
#include "geodesicresolver.h"
class AGDResoler :
	public GeodesicResolver
{
public:
	AGDResoler(void);
	~AGDResoler(void);
	DoubleProperty_ resolveGeo(WatertightMesh_ mesh);
};

