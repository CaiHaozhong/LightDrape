#pragma once
#include "Mesh.h"
#include "Skeleton.h"
#include "Segment.h"
#include "HoleInfo.h"
#include <fstream>
class WatertightMesh :
	public Mesh
{
private:
	Skeleton_ mSkeleton;	
	Mesh_ mOriginalMesh;		

	
	/* 每一个网格顶点有一个存储该顶点对应的骨骼节点下标的属性 */
	IntProperty_ mVertexPropertySKN;

	/* 每个网格顶点有一个存储该顶点与测地源点测地距离的属性 */
	DoubleProperty_ mVertexPropertyGeoDis;

protected:
	Segment_ mSegment;
public:
	WatertightMesh(Mesh_ mesh);
	~WatertightMesh(void);

	Skeleton_ getSkeleton() const;

	void setSkeleton(Skeleton_ val);	

	Segment_ getSegment() const;

	void setSegment(Segment_ val);

	/* 返回测地源点, 默认为位于中心最高点 */
	size_t getGeodesicSource();

	virtual Vec3d getAlignPoint();

	void alterOriginalMesh();

	/* 获取一个顶点的测地值 */
	double getGeodesicDis(size_t ver);

	IntProperty_ getVertexPropertySKN() const { return mVertexPropertySKN; }
	void setVertexPropertySKN(IntProperty_ val) { mVertexPropertySKN = val; }

	DoubleProperty_ getVertexPropertyGeoDis() const { return mVertexPropertyGeoDis; }
	void setVertexPropertyGeoDis(DoubleProperty_ val) { mVertexPropertyGeoDis = val; }

	/* 获取网格顶点对应的骨骼节点 */
	size_t getCorrSkeletonNode(size_t vertex);

	void dumpSkeLinkMesh(size_t skeNode, std::string sunPath);

	void dumpSkeleton(std::string name);

	Mesh_ getOriginalMesh() const { return mOriginalMesh; }

	int getOriginVertex(size_t watertightVertex);

	int getWatertightVertex(size_t originVertex);

private:
	void computeMap();
private:
	std::vector<int> mWatertightMapOrigin;
	std::vector<int> mOriginMapWatertight;
};
S_PTR(WatertightMesh);

