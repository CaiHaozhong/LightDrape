#pragma once
#include "Common.h"
#include <unordered_map>
class Region;
S_PTR(Region);
class Mesh;
S_PTR(Mesh);
class Mesher
{
public:
	Mesher(void);
	Mesher(Region_ region);
	~Mesher(void);

	void initWithRegion(Region_ region);

	Mesh_ getMesh();

	size_t getMeshVertex(size_t regionVertex);

	size_t getRegionVertex(size_t meshVertex);

private:
	/* 稀疏下标映射到紧密下标*/
	std::unordered_map<size_t, size_t > mSparseMapDense;

	/* 紧密下标映射到稀疏下标*/
	std::vector<size_t> mDenseMapSparse;

	Mesh_ mRetMesh;
};
S_PTR(Mesher);