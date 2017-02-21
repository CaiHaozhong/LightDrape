#pragma once
#include "Common.h"
#include <unordered_set>
class Region;
S_PTR(Region);
class MeshSegmenter;
S_PTR(MeshSegmenter);
class LevelSet;
S_PTR(LevelSet);
class LevelCircle;
S_PTR(LevelCircle);
class LevelNode;
S_PTR(LevelNode);
/* 进一步分割，将躯干中属于手臂的顶点划分到手臂上
 * 或者将躯干中属于袖子的顶点划分到袖子上
 */
class LeftTorseRightRefiner
{
protected:
	Region_ mLeft, mTorse, mRight;
	MeshSegmenter_ mMeshSegmenter;
private:
	static const int IN_LEFT = 0;
	static const int IN_RIGHT = 1;
	static const int IN_TORSE = 2;
public:
	LeftTorseRightRefiner(void);
	~LeftTorseRightRefiner(void);
	LeftTorseRightRefiner(MeshSegmenter_ meshSegmenter, Region_ left, Region_ torse, Region_ right);

	virtual void refine();

private:

	/* 获取LevelSet中的点 */
	void getVertexFromLevelSet(LevelSet_ ls, std::vector<size_t>& ret);

	/* 获取LevelSet中的点 */
	void getNodesFromLevelSet(LevelSet_ ls, std::vector<LevelNode_>& ret);

	/* 将Circle中的点加入到HashSet中 */
	void addCircleToHashSet(std::unordered_set<size_t>& ret, LevelCircle_ circle);

	/* 将Vertices中的点加入到HashSet中 */
	void addVerticesToHashSet(std::unordered_set<size_t>& ret, std::vector<size_t>& vertices );

	/* 确定一个顶点该位于哪个集合里 */
	int decideLocation(std::unordered_set<size_t>& leftSet, 
		std::unordered_set<size_t>& rightSet, 
		std::unordered_set<size_t>& torseSet,
		size_t v);

	/* 将一个Circle从头插入到一个Region中 */
	void insertCircleToRegion(Region_ region, LevelCircle_ circle);

protected:
	/* 从Region a中减去处于Region b中的顶点 */
	void regionSub(Region_ a, Region_ b);
};
S_PTR(LeftTorseRightRefiner);