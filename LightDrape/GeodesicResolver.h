#pragma once
#include "WatertightMesh.h"
#include "Geodesic/GeodesicAdapter.h"
class GeodesicResolver
{
public:
	GeodesicResolver(void);
	~GeodesicResolver(void);

	virtual DoubleProperty_ resolveGeo(WatertightMesh_ mesh);
	
	double getMinGeoDis() const;

	double getMaxGeoDis() const;

	/* �����ֵ��һ�� */
	void normalize(DoubleProperty_ geo, size_t count);

protected:
	double mMinDis, mMaxDis;
};
S_PTR(GeodesicResolver);