#pragma once
#include "WatertightMesh.h"
#include "Geodesic/GeodesicAdapter.h"
class GeodesicResolver
{
public:
	GeodesicResolver(void);
	~GeodesicResolver(void);

	virtual DoubleProperty_ resolveGeo(WatertightMesh_ mesh);
};
S_PTR(GeodesicResolver);