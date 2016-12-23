#pragma once
#include "Garment.h"
class Cloth :
	public Garment
{
public:
	~Cloth(void);
	size_t getGeodesicSource(){
		/* TO DO */
		return WatertightMesh::getGeodesicSource();
	}
};

