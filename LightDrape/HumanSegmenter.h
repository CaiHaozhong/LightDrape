#pragma once
#include "Meshsegmenter.h"
#include "Segment.h"
#include <unordered_set>
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

	bool mIsHead;
	double mLastLCRadius;
	size_t mCurLevelSet;
protected:
	/* Override */
	void onDifferentLevelSet(size_t seq, LevelSet_ levelSet);

	/* Override */
	void onFinishCoarseSegment();

	/* Override */
	Segment_ createSegment();

	/* Override */
	void onFinishSegmentHook();

	/* Override */
	void onBeginSegmentHook();

	/* Override */
	void refineSegment();

	void filterNoise(std::vector<bool>& isNoise);

private:
	void initHumanSegmenter(WatertightMesh_ mesh);

	void handleHead(LevelSet_ levelSet);

	void handleTorso(LevelSet_ levelSet);

	void handleLegs(LevelSet_ levelSet);

	bool isHead(LevelCircle_ lc);

	double maxRadius(LevelCircle_ lc);

	void refineHands();

	void refineLegs();

};

