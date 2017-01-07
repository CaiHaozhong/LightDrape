#pragma once
#include "geodesicresolver.h"
class GeodesicResolverCached :
	public GeodesicResolver
{
public:
	GeodesicResolverCached(void);
	~GeodesicResolverCached(void);

	DoubleProperty_ resolveGeo(WatertightMesh_ mesh);
private:
	std::string getGeoFileName(WatertightMesh_ mesh);
	bool hasGeoCached(WatertightMesh_ mesh);
	void cachedGeo(DoubleProperty_ pro, size_t vertexCount, std::string file);

	DoubleProperty_ readCached(Mesh_ mesh, std::string file);
};
S_PTR(GeodesicResolverCached);