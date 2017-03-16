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

	
	/* ÿһ�����񶥵���һ���洢�ö����Ӧ�Ĺ����ڵ��±������ */
	IntProperty_ mVertexPropertySKN;

	/* ÿ�����񶥵���һ���洢�ö�������Դ���ؾ�������� */
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

	/* ���ز��Դ��, Ĭ��Ϊλ��������ߵ� */
	size_t getGeodesicSource();

	virtual Vec3d getAlignPoint();

	void alterOriginalMesh();

	/* ��ȡһ������Ĳ��ֵ */
	double getGeodesicDis(size_t ver);

	IntProperty_ getVertexPropertySKN() const { return mVertexPropertySKN; }
	void setVertexPropertySKN(IntProperty_ val) { mVertexPropertySKN = val; }

	DoubleProperty_ getVertexPropertyGeoDis() const { return mVertexPropertyGeoDis; }
	void setVertexPropertyGeoDis(DoubleProperty_ val) { mVertexPropertyGeoDis = val; }

	/* ��ȡ���񶥵��Ӧ�Ĺ����ڵ� */
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

