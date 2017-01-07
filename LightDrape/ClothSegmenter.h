#pragma once
#include "Meshsegmenter.h"
#include "Common.h"
#include "ClothSegment.h"
#include "Region.h"
class ClothSegmenter :
	public MeshSegmenter
{
public:
	ClothSegmenter(WatertightMesh_ mesh);
	ClothSegmenter(void);

	void init(WatertightMesh_ mesh);
	
	~ClothSegmenter(void);
private:
	Region_ mTorso;
	Region_ mLeftSleeve;
	Region_ mRightSleeve;
	size_t mTorsoSkeNode, mLeftSleeveSkeNode, mRightSleeveSkeNode;
	WatertightMesh_ mMesh;
	Skeleton_ mMeshSkeleton;
protected:
	/* Override */
	void onDifferentLevelSet(size_t seq, LevelSet_ levelSet);

	/* Override */
	void onFinishCoarseSegment();

	/* Override */
	Segment_ createSegment();

private:
	void initClothSegmenter(WatertightMesh_ mesh);	

};

