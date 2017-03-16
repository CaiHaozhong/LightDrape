#pragma once
#include "meshsegmenter.h"
class TrousersSegmenter :
	public MeshSegmenter
{
public:
	TrousersSegmenter(WatertightMesh_ mesh);
	TrousersSegmenter(void);
	~TrousersSegmenter(void);

	void init(WatertightMesh_ mesh);

private:
	Region_ mTorso;
	Region_ mLeftLeg;
	Region_ mRightLeg;
	size_t mTorsoSkeNode, mLeftLegSkeNode, mRightLegSkeNode;
	WatertightMesh_ mMesh;
	Skeleton_ mMeshSkeleton;
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

private:
	void initTrousersSegmenter(WatertightMesh_ mesh);

	/* ay + bx = c */
	void pointsUnderLine(std::vector<size_t>& ret, 
		const Vec2d& p1, const Vec2d& p2, const Region_ region,
		const Region_ exclude);
};

