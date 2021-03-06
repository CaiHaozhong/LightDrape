#pragma once
#include <fstream>
#include <vector>
#include "WatertightMesh.h"
/* edge 是该Node所在的边的下标
 * start_vertex和factor用来计算
 * 该Node的位置
 * point(start_vertex) + factor * (point(end_vertex)-point(start_vertex))
 */
class LevelSet;
S_PTR(LevelSet);
class LevelNode{
public:
	size_t edge;
	size_t start_vertex;
	double factor;

	LevelNode();
	~LevelNode();

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
	LevelCircle();
	LevelCircle(LevelSet_ parent);
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

	/* 计算该圆的中心点 */
	Vec3d getMeanPoint(Mesh_ mesh);

	void setParent(LevelSet_ parent);

	LevelSet_ getParent() const;

	/* 获取该Circle的测地值 */
	double getHeight();
private:
	/* 判断两条边是否相邻 */
	bool isNeighbor(Mesh::EdgeHandle a, Mesh::EdgeHandle b, Mesh_ mesh);

	LevelSet_ mParent;
};
S_PTR(LevelCircle);
class LevelSet : public std::enable_shared_from_this<LevelSet>{	
private:
	/* 无序环形链表
	 * 容器一个
	 */
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

	/* 测地值 */
	double mHeight;
public:
	LevelSet();
	~LevelSet();
	LevelSet(WatertightMesh_ mesh);

	void setMesh(WatertightMesh_ mesh);

	WatertightMesh_ getMesh() const;

	/* 链表的形式存储节点 */
	void addNode(LevelNode_ node);

	void init();

	/* 获取Circle的个数 */
	size_t getCount();

	LevelCircle_ getCircle(size_t categoryIndex);

	double getHeight() const;
	
	void setHeight(double height);

	void dumpRaw( int i);
	void dump(int i);

	/* 将Level Set确切的点集dump到.cg中以供显示 */
	void dumpExactlyPoints( int i );

	std::vector<LevelCircle_>& getCircles();
private:
	/* 对该LevelSet上的点进行分类
	 * 比如分成左手、躯干、右手三个环
	 */
	void classify();

};
S_PTR(LevelSet);