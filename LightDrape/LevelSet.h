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
	size_t getNearestVertex(Mesh_ mesh);

	/* 该节点的确切位置
	 * point(start_vertex) + factor * (point(end_vertex)-point(start_vertex))
	 */
	Vec3d exactlyPoint(Mesh_ mesh);

	/* 计算与start_vertex对应的终点下标 */
	size_t getToVertexIndex(Mesh_ mesh);
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
	size_t mSkeNode;
	bool isCenterXValid;
	void addNode(LevelNode_ node);
	/* 判断新的LevelNode是否在这个圈里面
	 * 即，与这个圈里的任意一个节点相邻
	 */
	bool isInThisCircle(LevelNode_ newNode, Mesh_ mesh);

	/* 计算这个Circle的中心点的X坐标
	 * TODO: 也可以用骨骼节点来代替，可能更好
	 */
	void computeCenter(Mesh_ mesh);

	double getCenterX(Mesh_ mesh);
private:
	/* 判断两条边是否相邻 */
	bool isNeighbor(Mesh::EdgeHandle a, Mesh::EdgeHandle b, Mesh_ mesh);
};
S_PTR(LevelCircle);
class LevelSet{
private:
	/* 无序环形链表 */
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
		CircleLinkedList();
		/* 深复制 */
		CircleLinkedList(CircleLinkedList& other);
		~CircleLinkedList();

		/* 返回当前的节点值 */
		LevelNode_ cur();

		/* 将指针指向下一个节点 */
		void next();

		bool isEmpty();

		/* 删除一个节点，下一个节点位置不确定 */
		void removeCurrent();
		/* 添加一个节点 */
		void add(LevelNode_ node);

		size_t getCount();
	};
	
	WatertightMesh_ mMesh;
	std::vector<LevelCircle_> mCircles;
	CircleLinkedList mRawNodes;
	size_t mNodeCount;
public:
	LevelSet();
	~LevelSet();

	/* 链表的形式存储节点 */
	void addNode(LevelNode_ node);

	void init(WatertightMesh_ mesh);

	size_t getCount();

	LevelCircle_ getCircle(size_t categoryIndex);
	void dumpRaw(Mesh_ mesh, int i);
	void dump(int i);

private:
	/* 对该LevelSet上的点进行分类
	 * 比如分成左手、躯干、右手三个环
	 */
	void classify();

};
S_PTR(LevelSet);