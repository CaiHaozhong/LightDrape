#pragma once
#include <vector>
#include "Common.h"
#include <set>
#include "Vec3d.h"
class WatertightMesh;
S_PTR(WatertightMesh);
class Mesh;
S_PTR(Mesh);
class Skeleton;
S_PTR(Skeleton);
class LevelCircle;
S_PTR(LevelCircle);
class RegionSkeletonNode{
public:
	std::vector<size_t> vers;
	Vec3d center;
	double height;
	void dump(Mesh_ mesh, std::string file);
};
S_PTR(RegionSkeletonNode);

/* Region ���еĹ�����ͨ������LevelCircle�����ĵõ� */
class RegionSkeleton{
private:
	std::vector<RegionSkeletonNode_> mNodes;
public:
	void push_front(LevelCircle_ lc);
	void push_front(const std::vector<size_t>& vers, const Vec3d& center, double height);
	void push_back(LevelCircle_ lc);
	void push_back(const std::vector<size_t>& vers, const Vec3d& center, double height);
	RegionSkeletonNode_ start();
	size_t count() const;
	RegionSkeletonNode_ getNode(size_t index);
	void dump(Mesh_ mesh, std::string regionName);
private:
	void getVerticesFromCircle(LevelCircle_ circle, std::vector<size_t>& ret);
};
S_PTR(RegionSkeleton);
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

	/* ��Region�Ĺ�������LevelCircle�õ� */
	RegionSkeleton_ mSkeleton;

	/* ��Region����ɫ */
	Vec3uc mColor;

	/* ��Region������ */
	std::string mName;

public:
	Region(void);
	~Region(void);

	Region(WatertightMesh_ mesh, std::string name);

	void setColor(const Vec3uc& color);
	Vec3uc getColor() const;

	std::string getName() const { return mName; }
	void setName(std::string val) { mName = val; }

	void setMesh(WatertightMesh_ mesh);
	WatertightMesh_ getMesh();
	Skeleton_ getSkeleton();

	/* ������ʹ���������
	 * �������붥�㼯����δ�������
	 * ͨ�����������ӵĶ��㽫���ܹ�������
	 * ����ʹ��void addVertices(std::vector<size_t>& vers, Vec3d& center);
	 * ����size_t addCircle(LevelCircle_ circle);
	 */
	std::set<size_t>::iterator addVertex(size_t vertex);

	std::set<size_t>::iterator addSkeletonNode(size_t skenode);

	void removeVertex(size_t vertex);

	/* ����ԭʼ��LevelSet������������뵽�����Ͷ��㼯 */
	size_t addCircle(LevelCircle_ circle);

	/* ���Region�м��붥�㣬 centerΪ��Щ�����Ӧ��LevelCircle������
	 * һ��һ���ؼ���
	 */
	void addVertices(std::vector<size_t>& vers, Vec3d& center, double height);

	size_t getCircleCount() const;

	std::vector<LevelCircle_>& getCircles();

	/* ���ø�Region���ܵĵ���ʼ�����ڵ�
	 * ��Ϊ����ȥ��LevelCircle��Ӧ�Ĺ����ڵ����������ڵ�֮ǰ
	 * ���Ǹýڵ��Ѿ��ǳ��ӽ���ʼ�ڵ���
	 * ʹ��confirmStartSkeNode()��ȷ����������ʼ�����ڵ�
	*/
	void setPossibleStart(size_t start);
	
	/* ��ȡMean Curvature Skeleton����ʼ�ڵ� */
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

	void dumpSkeleton(std::string name);

	RegionSkeleton_ getRegionSkeleton() const;

	static Vec3d computeCenter(std::vector<size_t>& vers, Mesh_ mesh);

	void dumpRegionSkeleton(std::string regionName);
private:
	void confirmStartSkeNode();
	
	void expandVertices();
};
S_PTR(Region);