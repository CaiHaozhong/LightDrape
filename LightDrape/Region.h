#pragma once
#include <vector>
#include "Common.h"
#include "Skeleton.h"
#include <set>
#include <queue>
class Region
{
private:
	std::set<size_t> mVertics;
	std::set<size_t> mSkeNodes;
	Skeleton_ mSkeleton;

	/* 该Region的起始骨骼节点 */
	size_t mStartIndex;
public:
	Region(void);
	~Region(void);

	Region(Skeleton_ ske);
	void setSkeleton(Skeleton_ ske);

	void addVertex(size_t vertex);

	void addSkeleton(size_t skenode);

	/* 设置该Region的起始骨骼节点 */
	void setStart(size_t start);

	size_t getStart();

	std::set<size_t>& getVertices();;
	std::set<size_t>& getSkeNodes();

	/* 把骨骼节点间的节点加入该Region
	 * 并把骨骼节点对应的网格顶点加入该Region
	 * 对骨骼节点按照连接关系进行排序
	 */
	void expand();

private:
	void sortSkeleton();

	void expandVertices();
};
S_PTR(Region);