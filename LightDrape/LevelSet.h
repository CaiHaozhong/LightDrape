#pragma once
#include <fstream>
#include <vector>
#include "WatertightMesh.h"
/* edge 是该Node所在的边的下标
 * start_vertex和factor用来计算
 * 该Node的位置
 * point(start_vertex) + factor * (point(end_vertex)-point(start_vertex))
 */
class LevelNode{
public:
	size_t edge;
	size_t start_vertex;
	double factor;

	/* 获取和该Node位置最接近的网格顶点的下标 */
	size_t getNearestVertex(Mesh_ mesh){
		if(factor < 0.5){
			return start_vertex;
		}
		else{
			return getToVertexIndex(mesh);
		}
	}

	/* 该节点的确切位置
	 * point(start_vertex) + factor * (point(end_vertex)-point(start_vertex))
	 */
	Vec3d exactlyPoint(Mesh_ mesh){
		Mesh::VertexHandle start_handle = Mesh::VertexHandle(start_vertex);
		Mesh::VertexHandle end_handle = Mesh::VertexHandle(getToVertexIndex(mesh));
		Vec3d& startPoint = mesh->point(start_handle);
		Vec3d ret =  startPoint + factor * (mesh->point(end_handle)- startPoint);
		return ret;
	}

	/* 计算与start_vertex对应的终点下标 */
	size_t getToVertexIndex(Mesh_ mesh){
		OpenMesh::EdgeHandle edgeHandle = OpenMesh::EdgeHandle(edge);
		if(mesh->is_boundary(edgeHandle)){
			PRINTLN("Error! In LevelNode. Watertight mesh should not has boundary edge.");
			return start_vertex;
		}
		OpenMesh::HalfedgeHandle heHandle = mesh->halfedge_handle(edgeHandle, 0);
		size_t ret = mesh->to_vertex_handle(heHandle).idx();
		if(ret == start_vertex)
			ret = mesh->from_vertex_handle(heHandle).idx();
		return ret;
	}
};
S_PTR(LevelNode);

/* 一个独立的圈，这个圈里的节点与测地源点的测地距相等 */
class LevelCircle{
public:
	LevelCircle(){
		isCenterXValid = false;
	}
	std::vector<LevelNode_> levelNodes;
	double mCenterX;
	bool isCenterXValid;
	void addNode(LevelNode_ node){
		levelNodes.push_back(node);
	}
	/* 判断新的LevelNode是否在这个圈里面
	 * 即，与这个圈里的任意一个节点相邻
	 */
	bool isInThisCircle(LevelNode_ newNode, Mesh_ mesh){
		Mesh::EdgeHandle newEdge = Mesh::EdgeHandle(newNode->edge);
		for(size_t i = 0; i < levelNodes.size(); i++){
			if(isNeighbor(newEdge, Mesh::EdgeHandle(levelNodes[i]->edge), mesh)){
				return true;
			}
		}
		return false;
	}

	/* 计算这个Circle的中心点的X坐标
	 * TODO: 也可以用骨骼节点来代替，可能更好
	 */
	void computeCenter(Mesh_ mesh){
		double minX = std::numeric_limits<double>::max();
		double maxX = std::numeric_limits<double>::min();
		for(size_t i = 0; i < levelNodes.size(); i++){
			Vec3d exactPoint = levelNodes[i]->exactlyPoint(mesh);
			double epX = exactPoint.values_[0];
			if(epX > maxX){
				maxX = epX;
			}
			if(epX < minX){
				minX = epX;
			}
		}
		mCenterX = 0.5 * (maxX + minX);
		isCenterXValid = true;
	}

	double getCenterX(Mesh_ mesh){
		if(isCenterXValid == false){
			computeCenter(mesh);
		}
		return mCenterX;
	}
};
S_PTR(LevelCircle);
class LevelSet{
private:
	class CircleLinkedList{
	private:
		struct Node{
			LevelNode_ mNode;
			Node* next;
			Node(){
				next = nullptr;
			}
		};
		Node* mHead;
		size_t mCount;
		Node* mCur;
	public:
		CircleLinkedList(){
			mHead = nullptr;
			mCur = nullptr;
			mCount = 0;
		}
		void reset(){
			mCur = mHead;
		}
		LevelNode_ next(){
			if(isEmpty()){
				return nullptr;
			}
			if(mCur == nullptr){
				mCur = mHead;				
			}
			LevelNode_ ret = mCur->mNode;					
			mCur = mCur->next;
			return ret;			
		}
		bool isEmpty(){
			return mHead == nullptr;
		}
		void removeCurrent(){
			if(mCur == mHead){
				mHead = mHead->next;
			}
			Node* nextNode = mCur->next;
			delete mCur;			
			mCur = nextNode;
			mCount -= 1;
		}
		void add(LevelNode_ node){
			Node* n = new Node;
			n->mNode = node;
			n->next = mHead;
			mHead = n;
			mCount += 1;
		}
	};
	
	WatertightMesh_ mMesh;
	std::vector<LevelCircle_> mCircles;
	CircleLinkedList mRawNodes;
	size_t mNodeCount;
public:
	LevelSet(){
		mNodeCount = 0;		
	}
	~LevelSet(){}

	/* 链表的形式存储节点 */
	void addNode(LevelNode_ node){
		mRawNodes.add(node);
	}

	void init(WatertightMesh_ mesh){
		mMesh = mesh;		
		classify();		
	}

	size_t getCount(){
		return mCircles.size();
	}

	LevelCircle_ getCircle(size_t categoryIndex){
		if(categoryIndex >=0 && categoryIndex < mCircles.size())
			return mCircles.at(categoryIndex);
		else{
			PRINTLN("ERROR! In LevelSet. Index out of boundary.");
			return nullptr;
		}
	}

	void dump(int i){
#ifdef _DEBUG_
		std::string path = "../data/levelset/";
		char back[8];
		sprintf(back, "%d.cg", i);
		std::ofstream out = std::ofstream(path + mMesh->getName() + back);
		int edgeCount = 0;
		for(size_t i = 0; i < mCircles.size(); i++){
			edgeCount += mCircles[i]->levelNodes.size();
		}
		out << "# D:3 NV:" << mMesh->n_vertices() << " NE:" << edgeCount << "\n";
		for(Mesh::VertexIter vi = mMesh->vertices_begin(); vi != mMesh->vertices_end();
			vi++){
				Vec3d p = mMesh->point(*vi);
				out << "v " << p.values_[0] << " " << p.values_[1] << " " << p.values_[2] << "\n";
		}
		for(size_t i = 0; i < mCircles.size(); i++){
			LevelCircle_ lc = mCircles[i];
			for(size_t j = 0; j < lc->levelNodes.size(); j++){
				LevelNode_ ln = lc->levelNodes[j];
				auto vpair = mMesh->getEndVertices(Mesh::EdgeHandle(ln->edge));
				out << "e " << vpair.first+1 << " " << vpair.second+1 << "\n";
			}
		}
		out.close();
#endif
	}
private:
	/* 对该LevelSet上的点进行分类
	 * 比如分成左手、躯干、右手三个环
	 */
	void classify(){
		while(mRawNodes != nullptr){
			LevelCircle_ levelCircle = smartNew(LevelCircle);
			for(size_t i = 0; i < mRawNodes.size(); i++){
				
			}
			LevelNode_ ln = mRawNodes.back();						
			mRawNodes.pop_back();
		}
	}
	/* 判断两条边是否相邻 */
	bool isNeighbor(Mesh::EdgeHandle a, Mesh::EdgeHandle b, Mesh_ mesh){	
		std::pair<size_t, size_t> avs = mesh->getEndVertices(a);
		std::pair<size_t, size_t> bvs = mesh->getEndVertices(b);
		if(avs.first == bvs.first || avs.first == bvs.second
			|| avs.second == bvs.first || avs.second == bvs.second)
			return true;
		return false;
	}
};
S_PTR(LevelSet);