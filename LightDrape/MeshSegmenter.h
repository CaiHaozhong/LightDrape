#pragma once
#include "WatertightMesh.h"
#include "LevelSet.h"
#include <vector>
class MeshSegmenter
{
private:
	WatertightMesh_ mMesh;
	std::vector<LevelSet> mLevelSet;
public:
	MeshSegmenter(void);
	MeshSegmenter(WatertightMesh_ mesh){
		init(mesh);
	}
	~MeshSegmenter(void);
	void init(WatertightMesh_ mesh){
		mMesh = mesh;
	}


};

