#pragma once
#include "Common.h"
#include "Watertightmesh.h"
#include "Garment.h"
class Human :
	public WatertightMesh
{
public:
	~Human(void);
	size_t getGeodesicSource(){
		/* TO DO */
		return WatertightMesh::getGeodesicSource();
	}
	
	void dress(Garment garment){

	}
};
S_PTR(Human);