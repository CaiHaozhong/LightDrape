#pragma once
#include <queue>
#include <climits>
#include "Vec3d.h"
#include <vector>
#include "Common.h"
//#include "SkeletonNeckHunter.h" 和SkeletonNeckHunter交叉引用了

struct SkeletonNode{
	Vec3d point;
	std::vector<size_t> correspondanceIndices;
	Vec3d delta;
	SkeletonNode();
};
S_PTR(SkeletonNode);

struct SkeletonEdge{
	size_t sourceVertex;
	size_t targetVertex;
	SkeletonEdge(size_t s, size_t t);
	SkeletonEdge(){}
};
S_PTR(SkeletonEdge);

class Skeleton
{
public:
	typedef std::vector<size_t> IndexList;
	typedef std::vector<IndexList> AdjacencyList;
	typedef std::vector<SkeletonNode_> NodeList;
	typedef std::vector<SkeletonEdge_> EdgeList;
private:
	AdjacencyList mAdjacencyList;
	NodeList mNodeList;
	EdgeList mEdgeList;

	size_t mNeckIndex;
	IndexList mLeftHandVertices;
	IndexList mRightHandVertices;

public:
	Skeleton();
	~Skeleton();

	/* 返回插入的下标 */
	size_t addNode(SkeletonNode_ node);

	/* 返回插入的下标 */
	size_t addEdge(SkeletonEdge_ edge);

	const Skeleton::NodeList& getNodeList() { return mNodeList; }
	void setNodeList(Skeleton::NodeList val) { mNodeList = val; }
	const EdgeList& getEdgeList() { return mEdgeList; }
	void setEdgeList(EdgeList val) { mEdgeList = val; }

	/* 返回对应下标的节点 */
	SkeletonNode_ nodeAt(size_t i) const;

	/* 返回对应下标的边 */
	SkeletonEdge_ edgeAt(size_t i) const;

	/* 返回节点的个数 */
	size_t nodeCount() const{
		return mNodeList.size();
	}

	/* 返回边的个数 */
	size_t edgeCount() const{
		return mEdgeList.size();
	}

	/* 清空数据 */
	void clear();

	/* 判断两个节点是否相邻或同一个 */
	bool isNodeNeighbor(size_t node1, size_t node2);

	/* 计算两个节点之间节点的个数 */
	size_t intervalNodeCount(size_t node1, size_t node2);

	/* 计算两个节点间的欧氏距离 */
	double nodeEuclideanDis(size_t node1, size_t node2);

	IndexList& getNeighbors(size_t node){
		return mAdjacencyList[node];
	}

	void dump(std::string filename);
};

S_PTR(Skeleton);