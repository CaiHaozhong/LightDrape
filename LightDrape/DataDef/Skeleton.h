#ifndef __SKELETON__
#define __SKELETON__

#include <climits>
#include <OpenMesh/Core/Geometry/VectorT.hh>
#include <vector>
//#include "SkeletonNeckHunter.h" ��SkeletonNeckHunter����������

struct SkeletonNode{
	typedef OpenMesh::Vec3f Point;
	Point point;
	std::vector<size_t> correspondanceIndices;
	Point delta;
	SkeletonNode(){
		delta = Point(0,0,0);
	}
};
struct SkeletonEdge{
	size_t sourceVertex;
	size_t targetVertex;
	SkeletonEdge(size_t s, size_t t){
		sourceVertex = s;
		targetVertex = t;
	}
	SkeletonEdge(){}
};
class Skeleton
{
public:
	typedef std::vector<size_t> IndexList;
	typedef std::vector<IndexList> AdjacencyList;
	typedef std::vector<SkeletonNode*> NodeList;
	typedef std::vector<SkeletonEdge*> EdgeList;
private:
	AdjacencyList mAdjacencyList;
	NodeList mNodeList;
	EdgeList mEdgeList;

	size_t mNeckIndex;
	IndexList mLeftHandVertices;
	IndexList mRightHandVertices;

	//SkeletonNeckHunter* mSkeletonNeckHunter;
public:
	Skeleton(){}
	~Skeleton(){
		clear();
	}

	/* ���ز�����±� */
	size_t addNode(SkeletonNode* node){
		mNodeList.push_back(node);
		mAdjacencyList.push_back(IndexList());
		return mNodeList.size()-1;
	}

	/* ���ز�����±� */
	size_t addEdge(SkeletonEdge* edge){
		mEdgeList.push_back(edge);
		mAdjacencyList[edge->sourceVertex].push_back(edge->targetVertex);
		return mEdgeList.size()-1;
	}

	/* getter��setter */
	size_t getNeckIndex() const { return mNeckIndex; }
	void setNeckIndex(size_t val) { mNeckIndex = val; }
	Skeleton::NodeList getNodeList() const { return mNodeList; }
	void setNodeList(Skeleton::NodeList val) { mNodeList = val; }
	EdgeList getEdgeList() const { return mEdgeList; }
	void setEdgeList(EdgeList val) { mEdgeList = val; }

	/* ���ض�Ӧ�±�Ľڵ� */
	SkeletonNode* nodeAt(size_t i) const{
		if(i < mNodeList.size()){
			return mNodeList.at(i);
		}
		return nullptr;
	}

	/* ���ض�Ӧ�±�ı� */
	SkeletonEdge* edgeAt(size_t i) const{
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
		int numNode = mNodeList.size();
		for(size_t i = 0; i < numNode; i++){
			delete mNodeList.at(i);
		}
		mNodeList.clear();
		int numEdge = mEdgeList.size();
		for(size_t i = 0; i < numEdge; i++){
			delete mEdgeList.at(i);
		}
		mEdgeList.clear();
		mAdjacencyList.clear();
	}

	void findHand(){

	}

	IndexList& getLeftHandVerticesRef(){
		return mLeftHandVertices;
	}


	IndexList& getRightHandVerticesRef(){
		return mRightHandVertices;
	}

};
#endif // !__SKELETON__