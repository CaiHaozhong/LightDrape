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
/* ��һ���ָ�������������ֱ۵Ķ��㻮�ֵ��ֱ���
 * ���߽��������������ӵĶ��㻮�ֵ�������
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

	/* ��ȡLevelSet�еĵ� */
	void getVertexFromLevelSet(LevelSet_ ls, std::vector<size_t>& ret);

	/* ��ȡLevelSet�еĵ� */
	void getNodesFromLevelSet(LevelSet_ ls, std::vector<LevelNode_>& ret);

	/* ��Circle�еĵ���뵽HashSet�� */
	void addCircleToHashSet(std::unordered_set<size_t>& ret, LevelCircle_ circle);

	/* ��Vertices�еĵ���뵽HashSet�� */
	void addVerticesToHashSet(std::unordered_set<size_t>& ret, std::vector<size_t>& vertices );

	/* ȷ��һ�������λ���ĸ������� */
	int decideLocation(std::unordered_set<size_t>& leftSet, 
		std::unordered_set<size_t>& rightSet, 
		std::unordered_set<size_t>& torseSet,
		size_t v);

	/* ��һ��Circle��ͷ���뵽һ��Region�� */
	void insertCircleToRegion(Region_ region, LevelCircle_ circle);

protected:
	/* ��Region a�м�ȥ����Region b�еĶ��� */
	void regionSub(Region_ a, Region_ b);
};
S_PTR(LeftTorseRightRefiner);