#pragma once
#include <vector>
#include "Common.h"
#include <set>
#include "Vec3d.h"
class WatertightMesh;
S_PTR(WatertightMesh);
class Mesh;
S_PTR(Mesh);
class Skeleton;
S_PTR(Skeleton);
class LevelCircle;
S_PTR(LevelCircle);
class RegionSkeletonNode{
public:
	std::vector<size_t> vers;
	Vec3d center;
	double height;
	void dump(Mesh_ mesh, std::string file);
};
S_PTR(RegionSkeletonNode);

/* Region 独有的骨骼，通过计算LevelCircle的中心得到 */
class RegionSkeleton{
private:
	std::vector<RegionSkeletonNode_> mNodes;
public:
	void push_front(LevelCircle_ lc);
	void push_front(const std::vector<size_t>& vers, const Vec3d& center, double height);
	void push_back(LevelCircle_ lc);
	void push_back(const std::vector<size_t>& vers, const Vec3d& center, double height);
	RegionSkeletonNode_ start();
	size_t count() const;
	RegionSkeletonNode_ getNode(size_t index);
	void dump(Mesh_ mesh, std::string regionName);
private:
	void getVerticesFromCircle(LevelCircle_ circle, std::vector<size_t>& ret);
};
S_PTR(RegionSkeleton);
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

	/* 该Region的骨骼，由LevelCircle得到 */
	RegionSkeleton_ mSkeleton;

	/* 该Region的颜色 */
	Vec3uc mColor;

	/* 该Region的名字 */
	std::string mName;

public:
	Region(void);
	~Region(void);

	Region(WatertightMesh_ mesh, std::string name);

	void setColor(const Vec3uc& color);
	Vec3uc getColor() const;

	std::string getName() const { return mName; }
	void setName(std::string val) { mName = val; }

	void setMesh(WatertightMesh_ mesh);
	WatertightMesh_ getMesh();
	Skeleton_ getSkeleton();

	/* 尽量不使用这个函数
	 * 仅仅加入顶点集，并未加入骨骼
	 * 通过这个函数添加的顶点将不受骨骼控制
	 * 可以使用void addVertices(std::vector<size_t>& vers, Vec3d& center);
	 * 或者size_t addCircle(LevelCircle_ circle);
	 */
	std::set<size_t>::iterator addVertex(size_t vertex);

	std::set<size_t>::iterator addSkeletonNode(size_t skenode);

	void removeVertex(size_t vertex);

	/* 保存原始的LevelSet，并将顶点加入到骨骼和顶点集 */
	size_t addCircle(LevelCircle_ circle);

	/* 向该Region中加入顶点， center为这些顶点对应的LevelCircle的中心
	 * 一环一环地加入
	 */
	void addVertices(std::vector<size_t>& vers, Vec3d& center, double height);

	size_t getCircleCount() const;

	std::vector<LevelCircle_>& getCircles();

	/* 设置该Region可能的的起始骨骼节点
	 * 因为接下去的LevelCircle对应的骨骼节点可能在这个节点之前
	 * 但是该节点已经非常接近起始节点了
	 * 使用confirmStartSkeNode()来确定真正的起始骨骼节点
	*/
	void setPossibleStart(size_t start);
	
	/* 获取Mean Curvature Skeleton的起始节点 */
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

	void dumpSkeleton(std::string name);

	RegionSkeleton_ getRegionSkeleton() const;

	static Vec3d computeCenter(std::vector<size_t>& vers, Mesh_ mesh);

	void dumpRegionSkeleton(std::string regionName);
private:
	void confirmStartSkeNode();
	
	void expandVertices();
};
S_PTR(Region);