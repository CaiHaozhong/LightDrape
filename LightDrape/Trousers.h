#pragma once
#include "garment.h"
class Trousers :
	public Garment
{
public:
	~Trousers(void);
	size_t getGeodesicSource(){
		/* TO DO */
		return WatertightMesh::getGeodesicSource();
	}
};

