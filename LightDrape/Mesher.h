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
	/* ϡ���±�ӳ�䵽�����±�*/
	std::unordered_map<size_t, size_t > mSparseMapDense;

	/* �����±�ӳ�䵽ϡ���±�*/
	std::vector<size_t> mDenseMapSparse;

	Mesh_ mRetMesh;
};
S_PTR(Mesher);