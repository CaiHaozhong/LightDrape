#pragma once
#include "Common.h"
#include <vector>
class PenetrationResolver;
S_PTR(PenetrationResolver);
class Mesh;
S_PTR(Mesh);
class GarmentPenetrationResolver
{
private:
	PenetrationResolver_ mPenetrationResolver;
	Mesh_ mGarment, mHuman;
public:
	GarmentPenetrationResolver(void);

	GarmentPenetrationResolver(Mesh_ garment);

	~GarmentPenetrationResolver(void);

	void setHuman(Mesh_ human);

	void setGarment(Mesh_ garment);

	bool resolve();

private:
	/* 计算网格顶点的邻接表 */
	void computeAdjList( Mesh_ mesh, std::vector< std::vector<size_t> >& adjList );
};
S_PTR(GarmentPenetrationResolver);

