#pragma once
#include <queue>
#include <climits>
#include "Vec3d.h"
#include <vector>
#include "Common.h"
//#include "SkeletonNeckHunter.h" ��SkeletonNeckHunter����������

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

	/* ���ز�����±� */
	size_t addNode(SkeletonNode_ node);

	/* ���ز�����±� */
	size_t addEdge(SkeletonEdge_ edge);

	const Skeleton::NodeList& getNodeList() { return mNodeList; }
	void setNodeList(Skeleton::NodeList val) { mNodeList = val; }
	const EdgeList& getEdgeList() { return mEdgeList; }
	void setEdgeList(EdgeList val) { mEdgeList = val; }

	/* ���ض�Ӧ�±�Ľڵ� */
	SkeletonNode_ nodeAt(size_t i) const;

	/* ���ض�Ӧ�±�ı� */
	SkeletonEdge_ edgeAt(size_t i) const;

	/* ���ؽڵ�ĸ��� */
	size_t nodeCount() const{
		return mNodeList.size();
	}

	/* ���رߵĸ��� */
	size_t edgeCount() const{
		return mEdgeList.size();
	}

	/* ������� */
	void clear();

	/* �ж������ڵ��Ƿ����ڻ�ͬһ�� */
	bool isNodeNeighbor(size_t node1, size_t node2);

	/* ���������ڵ�֮��ڵ�ĸ��� */
	size_t intervalNodeCount(size_t node1, size_t node2);

	/* ���������ڵ���ŷ�Ͼ��� */
	double nodeEuclideanDis(size_t node1, size_t node2);

	IndexList& getNeighbors(size_t node){
		return mAdjacencyList[node];
	}

	void dump(std::string filename);
};

S_PTR(Skeleton);