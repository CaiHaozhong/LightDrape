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
	/* Level Set之间的间隔 */
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


	/* 开始分割，并将分割结果存储在Mesh中 */
	void segment(){
		mSegment = createSegment();
		coarseSegment();
		refineSegment();
		mMesh->setSegment(mSegment);
	}
private:
	/* 根据网格边的长度决定Level Set的间隔 */
	void decideGranularity(){
		mGranularity = 20;
	}

	void computeLevelSet(){

	}

protected:
	/* 不同的模型，分割的方法不同 */
	virtual void coarseSegment() = 0;

	/* 不同的模型，有不同的分割 */
	virtual Segment_ createSegment() = 0;
};

