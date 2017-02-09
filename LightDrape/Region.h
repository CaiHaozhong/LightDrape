#pragma once
#include <vector>
#include "Common.h"
#include <set>
class WatertightMesh;
S_PTR(WatertightMesh);
class Skeleton;
S_PTR(Skeleton);
class LevelCircle;
S_PTR(LevelCircle);
class Region
{
private:
	std::set<size_t> mVertics;
	std::set<size_t> mSkeNodes;
	std::vector<LevelCircle_> mLevelCircles;
	WatertightMesh_ mMesh;

	/* 该Region的起始骨骼节点 */
	size_t mStartSkeIndex;
	bool mHasStartSetted;
public:
	Region(void);
	~Region(void);

	Region(WatertightMesh_ mesh);

	void setMesh(WatertightMesh_ mesh);
	WatertightMesh_ getMesh();
	Skeleton_ getSkeleton();

	std::set<size_t>::iterator addVertex(size_t vertex);

	std::set<size_t>::iterator addSkeletonNode(size_t skenode);

	/* 保存原始的LevelSet */
	size_t addCircle(LevelCircle_ circle);

	size_t getCircleCount() const;

	std::vector<LevelCircle_>& getCircles();

	/* 设置该Region可能的的起始骨骼节点
	 * 因为接下去的LevelCircle对应的骨骼节点可能在这个节点之前
	 * 但是该节点已经非常接近起始节点了
	 * 使用confirmStartSkeNode()来确定真正的其实骨骼节点
	*/
	void setPossibleStart(size_t start);
	
	size_t getStart();

	/* 是否已经设置了可能的起始骨骼节点 */
	bool hasStartSetted();

	std::set<size_t>& getVertices();
	std::set<size_t>& getSkeNodes();

	/* 获取该Region的顶点的个数 */
	size_t getVerticesSize() const;

	/* 把骨骼节点间的节点加入该Region
	 * 并把骨骼节点对应的网格顶点加入该Region	
	 */
	void expand();

	void dump(std::string name);
private:
	void confirmStartSkeNode();
	
	void expandVertices();
};
S_PTR(Region);