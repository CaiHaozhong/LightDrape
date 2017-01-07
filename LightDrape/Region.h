#pragma once
#include <vector>
#include "Common.h"
#include "Skeleton.h"
#include <set>
#include <queue>
class MultiNextNodeHandler{
protected:
	Skeleton_ mSkeleton;
public:
	MultiNextNodeHandler(Skeleton_ skeleton){
		mSkeleton = skeleton;
	}
	virtual size_t decide(size_t prev, std::vector<size_t>& nextNodes) = 0;
};
S_PTR(MultiNextNodeHandler);
class ChooseLongBranch : public MultiNextNodeHandler{
public:
	ChooseLongBranch(Skeleton_ skeleton):MultiNextNodeHandler(skeleton){}

	size_t decide(size_t prev, std::vector<size_t>& nextNodes);
private:
	size_t length(size_t prev, size_t node);
};
S_PTR(ChooseLongBranch);
class Region
{
private:
	std::set<size_t> mVertics;
	std::set<size_t> mSkeNodes;
	Skeleton_ mSkeleton;

	/* 该Region的起始骨骼节点 */
	size_t mStartSkeIndex;
	bool mHasStartSetted;
public:
	Region(void);
	~Region(void);

	Region(Skeleton_ ske);
	void setSkeleton(Skeleton_ ske);

	std::set<size_t>::iterator addVertex(size_t vertex);

	std::set<size_t>::iterator addSkeleton(size_t skenode);

	/* 设置该Region可能的的起始骨骼节点
	 * 因为接下去的LevelCircle对应的骨骼节点可能在这个节点之前
	 * 但是该节点已经非常接近起始节点了
	 * 使用confirmStartSkeNode()来确定真正的其实骨骼节点
	*/
	void setPossibleStart(size_t start);
	
	size_t getStart();

	/* 是否已经设置了可能的起始骨骼节点 */
	bool hasStartSetted();

	std::set<size_t>& getVertices();;
	std::set<size_t>& getSkeNodes();

	/* 把骨骼节点间的节点加入该Region
	 * 并把骨骼节点对应的网格顶点加入该Region	
	 */
	void expand();

	/* 对骨骼节点按照连接关系进行排序
	 * 参数：skeNodes 输出
	 * 参数：multiNextNodeHandler 当一个节点的下一个节点有多个可选时，由这个对象
	 * 决定选哪个
	 */
	void toSortedSkeleton(std::vector<size_t>& skeNodes, 
		MultiNextNodeHandler_ multiNextNodeHandler);

	void dump(std::string name);
private:
	void confirmStartSkeNode();

	void sortSkeleton();
	
	void expandVertices();
};
S_PTR(Region);