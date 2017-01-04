#pragma once
#include <fstream>
#include <vector>
#include "WatertightMesh.h"
/* edge �Ǹ�Node���ڵıߵ��±�
 * start_vertex��factor��������
 * ��Node��λ��
 * point(start_vertex) + factor * (point(end_vertex)-point(start_vertex))
 */
class LevelNode{
public:
	size_t edge;
	size_t start_vertex;
	double factor;

	/* ��ȡ�͸�Nodeλ����ӽ������񶥵���±� */
	size_t getNearestVertex(Mesh_ mesh);

	/* �ýڵ��ȷ��λ��
	 * point(start_vertex) + factor * (point(end_vertex)-point(start_vertex))
	 */
	Vec3d exactlyPoint(Mesh_ mesh);

	/* ������start_vertex��Ӧ���յ��±� */
	size_t getToVertexIndex(Mesh_ mesh);
};
S_PTR(LevelNode);

/* һ��������Ȧ�����Ȧ��Ľڵ�����Դ��Ĳ�ؾ���� */
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
	/* �ж��µ�LevelNode�Ƿ������Ȧ����
	 * ���������Ȧ�������һ���ڵ�����
	 */
	bool isInThisCircle(LevelNode_ newNode, Mesh_ mesh);

	/* �������Circle�����ĵ��X����
	 * TODO: Ҳ�����ù����ڵ������棬���ܸ���
	 */
	void computeCenter(Mesh_ mesh);

	double getCenterX(Mesh_ mesh);
private:
	/* �ж��������Ƿ����� */
	bool isNeighbor(Mesh::EdgeHandle a, Mesh::EdgeHandle b, Mesh_ mesh);
};
S_PTR(LevelCircle);
class LevelSet{
private:
	/* ���������� */
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
		/* ��� */
		CircleLinkedList(CircleLinkedList& other);
		~CircleLinkedList();

		/* ���ص�ǰ�Ľڵ�ֵ */
		LevelNode_ cur();

		/* ��ָ��ָ����һ���ڵ� */
		void next();

		bool isEmpty();

		/* ɾ��һ���ڵ㣬��һ���ڵ�λ�ò�ȷ�� */
		void removeCurrent();
		/* ���һ���ڵ� */
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

	/* �������ʽ�洢�ڵ� */
	void addNode(LevelNode_ node);

	void init(WatertightMesh_ mesh);

	size_t getCount();

	LevelCircle_ getCircle(size_t categoryIndex);
	void dumpRaw(Mesh_ mesh, int i);
	void dump(int i);

private:
	/* �Ը�LevelSet�ϵĵ���з���
	 * ����ֳ����֡����ɡ�����������
	 */
	void classify();

};
S_PTR(LevelSet);