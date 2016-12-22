#pragma once

#include <climits>
#include "Vec3d.h"
#include <vector>
#include "Common.h"
//#include "SkeletonNeckHunter.h" 和SkeletonNeckHunter交叉引用了

struct SkeletonNode{
	Vec3d point;
	std::vector<size_t> correspondanceIndices;
	Vec3d delta;
	SkeletonNode(){
		delta = Vec3d(0,0,0);
	}
};
S_PTR(SkeletonNode);

struct SkeletonEdge{
	size_t sourceVertex;
	size_t targetVertex;
	SkeletonEdge(size_t s, size_t t){
		sourceVertex = s;
		targetVertex = t;
	}
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
	Skeleton(){}
	~Skeleton(){
		clear();
	}

	/* 返回插入的下标 */
	size_t addNode(SkeletonNode_ node){
		mNodeList.push_back(node);
		mAdjacencyList.push_back(IndexList());
		return mNodeList.size()-1;
	}

	/* 返回插入的下标 */
	size_t addEdge(SkeletonEdge_ edge){
		mEdgeList.push_back(edge);
		mAdjacencyList[edge->sourceVertex].push_back(edge->targetVertex);
		return mEdgeList.size()-1;
	}

	const Skeleton::NodeList& getNodeList() { return mNodeList; }
	void setNodeList(Skeleton::NodeList val) { mNodeList = val; }
	const EdgeList& getEdgeList() { return mEdgeList; }
	void setEdgeList(EdgeList val) { mEdgeList = val; }

	/* 返回对应下标的节点 */
	SkeletonNode_ nodeAt(size_t i) const{
		if(i < mNodeList.size()){
			return mNodeList.at(i);
		}
		return nullptr;
	}

	/* 返回对应下标的边 */
	SkeletonEdge_ edgeAt(size_t i) const{
		if(i < mEdgeList.size()){
			return mEdgeList.at(i);
		}
		return nullptr;
	}

	/* 返回节点的个数 */
	size_t nodeCount() const{
		return mNodeList.size();
	}

	/* 返回边的个数 */
	size_t edgeCount() const{
		return mEdgeList.size();
	}

	/* 清空数据 */
	void clear(){
		mNodeList.clear();
		mEdgeList.clear();
		mAdjacencyList.clear();
	}

};

S_PTR(Skeleton);