#include "GeodesicResolver.h"


GeodesicResolver::GeodesicResolver(void)
{
}


GeodesicResolver::~GeodesicResolver(void)
{
}

DoubleProperty_ GeodesicResolver::resolveGeo( WatertightMesh_ mesh )
{
	DoubleProperty_ ret = smartNew(DoubleProperty);
	mesh->registerProperty(ret);
	GeodesicAdapter geodesicAdapter(*mesh);
	size_t s = mesh->getGeodesicSource();
	geodesicAdapter.addSource(s);
	std::vector<double> result;
	geodesicAdapter.computeGeodesicFromSource(result);
	for(size_t i = 0; i < result.size(); i++){
		ret->set(i, result.at(i));
	}
	return ret;
}
