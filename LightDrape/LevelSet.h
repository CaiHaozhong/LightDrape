#pragma once
#include <vector>
#include "WatertightMesh.h"
class LevelSet{
private:
	WatertightMesh_ mMesh;
	std::vector< std::vector<size_t> > mEdges;
public:
	LevelSet(){}
	~LevelSet(){}
	void init(WatertightMesh_ mesh, std::vector<size_t>& edges){
		mEdges.push_back(edges);
		mMesh = mesh;
	}
	size_t classify(){

	}

	size_t getCount(){
		return mEdges.size();
	}
};