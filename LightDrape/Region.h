#pragma once
#include <vector>
#include "Common.h"
#include <set>
class WatertightMesh;
S_PTR(WatertightMesh);
class Skeleton;
S_PTR(Skeleton);
class LevelCircle;
S_PTR(LevelCircle);
class Region
{
private:
	std::set<size_t> mVertics;
	std::set<size_t> mSkeNodes;
	std::vector<LevelCircle_> mLevelCircles;
	WatertightMesh_ mMesh;

	/* ��Region����ʼ�����ڵ� */
	size_t mStartSkeIndex;
	bool mHasStartSetted;
public:
	Region(void);
	~Region(void);

	Region(WatertightMesh_ mesh);

	void setMesh(WatertightMesh_ mesh);
	WatertightMesh_ getMesh();
	Skeleton_ getSkeleton();

	std::set<size_t>::iterator addVertex(size_t vertex);

	std::set<size_t>::iterator addSkeletonNode(size_t skenode);

	/* ����ԭʼ��LevelSet */
	size_t addCircle(LevelCircle_ circle);

	size_t getCircleCount() const;

	std::vector<LevelCircle_>& getCircles();

	/* ���ø�Region���ܵĵ���ʼ�����ڵ�
	 * ��Ϊ����ȥ��LevelCircle��Ӧ�Ĺ����ڵ����������ڵ�֮ǰ
	 * ���Ǹýڵ��Ѿ��ǳ��ӽ���ʼ�ڵ���
	 * ʹ��confirmStartSkeNode()��ȷ����������ʵ�����ڵ�
	*/
	void setPossibleStart(size_t start);
	
	size_t getStart();

	/* �Ƿ��Ѿ������˿��ܵ���ʼ�����ڵ� */
	bool hasStartSetted();

	std::set<size_t>& getVertices();
	std::set<size_t>& getSkeNodes();

	/* ��ȡ��Region�Ķ���ĸ��� */
	size_t getVerticesSize() const;

	/* �ѹ����ڵ��Ľڵ�����Region
	 * ���ѹ����ڵ��Ӧ�����񶥵�����Region	
	 */
	void expand();

	void dump(std::string name);
private:
	void confirmStartSkeNode();
	
	void expandVertices();
};
S_PTR(Region);