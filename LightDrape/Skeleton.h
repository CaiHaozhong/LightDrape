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

	/* ���ز�����±� */
	size_t addNode(SkeletonNode_ node){
		mNodeList.push_back(node);
		mAdjacencyList.push_back(IndexList());
		return mNodeList.size()-1;
	}

	/* ���ز�����±� */
	size_t addEdge(SkeletonEdge_ edge){
		mEdgeList.push_back(edge);
		mAdjacencyList[edge->sourceVertex].push_back(edge->targetVertex);
		mAdjacencyList[edge->targetVertex].push_back(edge->sourceVertex);
		return mEdgeList.size()-1;
	}

	const Skeleton::NodeList& getNodeList() { return mNodeList; }
	void setNodeList(Skeleton::NodeList val) { mNodeList = val; }
	const EdgeList& getEdgeList() { return mEdgeList; }
	void setEdgeList(EdgeList val) { mEdgeList = val; }

	/* ���ض�Ӧ�±�Ľڵ� */
	SkeletonNode_ nodeAt(size_t i) const{
		if(i < mNodeList.size()){
			return mNodeList.at(i);
		}
		return nullptr;
	}

	/* ���ض�Ӧ�±�ı� */
	SkeletonEdge_ edgeAt(size_t i) const{
		if(i < mEdgeList.size()){
			return mEdgeList.at(i);
		}
		return nullptr;
	}

	/* ���ؽڵ�ĸ��� */
	size_t nodeCount() const{
		return mNodeList.size();
	}

	/* ���رߵĸ��� */
	size_t edgeCount() const{
		return mEdgeList.size();
	}

	/* ������� */
	void clear(){
		mNodeList.clear();
		mEdgeList.clear();
		mAdjacencyList.clear();
	}

	/* �ж������ڵ��Ƿ����ڻ�ͬһ�� */
	bool isNodeNeighbor(size_t node1, size_t node2){
		if(node1 == node2){
			return true;
		}
		else{
			IndexList& neighbors = mAdjacencyList[node1];
			if(std::find(neighbors.begin(), neighbors.end(), node2)
				!= neighbors.end()){
					return true;
			}
		}
		return false;
	}

	/* ���������ڵ�֮��ڵ�ĸ��� */
	size_t nodeDis(size_t node1, size_t node2){
		if(node1 == node2){
			return 0;
		}
		bool* hasVisited = new bool[this->nodeCount()];
		size_t* steps = new size_t[this->nodeCount()];
		memset(hasVisited, 0, this->nodeCount());
		memset(steps, 0, this->nodeCount());
		std::queue<size_t> Q;
		hasVisited[node1] = true;
		steps[node1] = 0;
		Q.push(node1);
		while(!Q.empty()){
			size_t cur = Q.front();
			Q.pop();
			IndexList& neighs = mAdjacencyList[cur];
			for(size_t i = 0; i < neighs.size(); i++){
				size_t n = neighs[i];
				if(hasVisited[n]){
					continue;
				}
				if(n == node2){					
					size_t ret = steps[cur]+1;
					delete [] hasVisited;
					delete [] steps;
					return ret;
				}
				else{
					steps[n] = steps[cur]+1;
				}
				hasVisited[n] = true;
				Q.push(n);
			}
		}
		delete [] hasVisited;
		delete [] steps;
		return -1;
	}

	IndexList& getNeighbors(size_t node){
		return mAdjacencyList[node];
	}
};

S_PTR(Skeleton);