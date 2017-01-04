#pragma once
#include <vector>
#include "Common.h"
#include "Skeleton.h"
#include <set>
#include <queue>
class Region
{
private:
	std::set<size_t> mVertics;
	std::set<size_t> mSkeNodes;
	Skeleton_ mSkeleton;

	/* ��Region����ʼ�����ڵ� */
	size_t mStartIndex;
public:
	Region(void);
	~Region(void);

	Region(Skeleton_ ske);
	void setSkeleton(Skeleton_ ske);

	void addVertex(size_t vertex);

	void addSkeleton(size_t skenode);

	/* ���ø�Region����ʼ�����ڵ� */
	void setStart(size_t start);

	size_t getStart();

	std::set<size_t>& getVertices();;
	std::set<size_t>& getSkeNodes();

	/* �ѹ����ڵ��Ľڵ�����Region
	 * ���ѹ����ڵ��Ӧ�����񶥵�����Region
	 * �Թ����ڵ㰴�����ӹ�ϵ��������
	 */
	void expand();

private:
	void sortSkeleton();

	void expandVertices();
};
S_PTR(Region);