#pragma once
#include <vector>
#include "Common.h"
#include "Skeleton.h"
#include <set>
#include <queue>
class MultiNextNodeHandler{
protected:
	Skeleton_ mSkeleton;
public:
	MultiNextNodeHandler(Skeleton_ skeleton){
		mSkeleton = skeleton;
	}
	virtual size_t decide(size_t prev, std::vector<size_t>& nextNodes) = 0;
};
S_PTR(MultiNextNodeHandler);
class ChooseLongBranch : public MultiNextNodeHandler{
public:
	ChooseLongBranch(Skeleton_ skeleton):MultiNextNodeHandler(skeleton){}

	size_t decide(size_t prev, std::vector<size_t>& nextNodes);
private:
	size_t length(size_t prev, size_t node);
};
S_PTR(ChooseLongBranch);
class Region
{
private:
	std::set<size_t> mVertics;
	std::set<size_t> mSkeNodes;
	Skeleton_ mSkeleton;

	/* ��Region����ʼ�����ڵ� */
	size_t mStartSkeIndex;
	bool mHasStartSetted;
public:
	Region(void);
	~Region(void);

	Region(Skeleton_ ske);
	void setSkeleton(Skeleton_ ske);

	std::set<size_t>::iterator addVertex(size_t vertex);

	std::set<size_t>::iterator addSkeleton(size_t skenode);

	/* ���ø�Region���ܵĵ���ʼ�����ڵ�
	 * ��Ϊ����ȥ��LevelCircle��Ӧ�Ĺ����ڵ����������ڵ�֮ǰ
	 * ���Ǹýڵ��Ѿ��ǳ��ӽ���ʼ�ڵ���
	 * ʹ��confirmStartSkeNode()��ȷ����������ʵ�����ڵ�
	*/
	void setPossibleStart(size_t start);
	
	size_t getStart();

	/* �Ƿ��Ѿ������˿��ܵ���ʼ�����ڵ� */
	bool hasStartSetted();

	std::set<size_t>& getVertices();;
	std::set<size_t>& getSkeNodes();

	/* �ѹ����ڵ��Ľڵ�����Region
	 * ���ѹ����ڵ��Ӧ�����񶥵�����Region	
	 */
	void expand();

	/* �Թ����ڵ㰴�����ӹ�ϵ��������
	 * ������skeNodes ���
	 * ������multiNextNodeHandler ��һ���ڵ����һ���ڵ��ж����ѡʱ�����������
	 * ����ѡ�ĸ�
	 */
	void toSortedSkeleton(std::vector<size_t>& skeNodes, 
		MultiNextNodeHandler_ multiNextNodeHandler);

	void dump(std::string name);
private:
	void confirmStartSkeNode();

	void sortSkeleton();
	
	void expandVertices();
};
S_PTR(Region);