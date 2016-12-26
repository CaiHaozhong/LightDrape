#pragma once
#include "WatertightMesh.h"
#include "LevelSet.h"
#include "GeodesicResolver.h"
#include "Segment.h"
#include <vector>
class MeshSegmenter
{
private:
	WatertightMesh_ mMesh;
	std::vector<LevelSet> mLevelSet;
	DoubleProperty_ mGeodisPropery;
	Segment_ mSegment;
	/* Level Set֮��ļ�� */
	double mGranularity;
public:
	MeshSegmenter(void);
	MeshSegmenter(WatertightMesh_ mesh){
		init(mesh);		
	}
	~MeshSegmenter(void);
	void init(WatertightMesh_ mesh){
		mMesh = mesh;
		decideGranularity();
		GeodesicResolver geodesicResolver;
		mGeodisPropery = geodesicResolver.resolveGeo(mMesh);
		computeLevelSet();
	}
	
	void refineSegment();


	/* ��ʼ�ָ�����ָ����洢��Mesh�� */
	void segment(){
		mSegment = createSegment();
		coarseSegment();
		refineSegment();
		mMesh->setSegment(mSegment);
	}
private:
	/* ��������ߵĳ��Ⱦ���Level Set�ļ�� */
	void decideGranularity(){
		mGranularity = 20;
	}

	void computeLevelSet(){

	}

protected:
	/* ��ͬ��ģ�ͣ��ָ�ķ�����ͬ */
	virtual void coarseSegment() = 0;

	/* ��ͬ��ģ�ͣ��в�ͬ�ķָ� */
	virtual Segment_ createSegment() = 0;
};

