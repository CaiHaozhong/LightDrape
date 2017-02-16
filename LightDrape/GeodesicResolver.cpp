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
	mesh->setVertexPropertyGeoDis(ret);
	GeodesicAdapter geodesicAdapter(*mesh);
	size_t s = mesh->getGeodesicSource();
	geodesicAdapter.addSource(s);
	std::vector<double> result;
	geodesicAdapter.computeGeodesicFromSource(result);
	mMaxDis = std::numeric_limits<double>::min();
	mMinDis = std::numeric_limits<double>::max();
	for(size_t i = 0; i < result.size(); i++){
		double v = result[i];
		ret->set(i, v);
		if(v > mMaxDis){
			mMaxDis = v;
		}
		if(v < mMinDis){
			mMinDis = v;
		}
	}
	return ret;
}

void GeodesicResolver::normalize( DoubleProperty_ geo, size_t count)
{
	double len = mMaxDis - mMinDis;
	for(size_t i = 0; i < count; i++){
		geo->set(i, (geo->get(i)-mMinDis)/len);
	}	
}

double GeodesicResolver::getMinGeoDis() const
{
	return mMinDis;
}

double GeodesicResolver::getMaxGeoDis() const
{
	return mMaxDis;
}
