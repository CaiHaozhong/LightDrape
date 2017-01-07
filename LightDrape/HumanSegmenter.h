#pragma once
#include "Meshsegmenter.h"
#include "Segment.h"
class HumanSegmenter :
	public MeshSegmenter
{
public:
	HumanSegmenter(WatertightMesh_ mesh);
	HumanSegmenter(void);

	void init(WatertightMesh_ mesh);

	~HumanSegmenter(void);
private:
	Region_ mHead;
	Region_ mTorso;
	Region_ mLeftHand;
	Region_ mRightHand;
	Region_ mLeftLeg;
	Region_ mRightLeg;
	size_t mTorsoSkeNode, mLeftHandSkeNode, mRightHandSkeNode
		, mLeftLegSkeNode, mRightLegSkeNode;
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
	void initHumanSegmenter(WatertightMesh_ mesh);

	void handleHead(LevelSet_ levelSet);


	void handleTorso(LevelSet_ levelSet);


	void handleLegs(LevelSet_ levelSet);
};

