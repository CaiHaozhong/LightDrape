#pragma once
#include "skeletonfitter.h"
#include <set>
class Mesh;
S_PTR(Mesh);
class Skeleton;
S_PTR(Skeleton);
class RegionSkeletonizer{
public:
	Skeleton_ skeletonize(Region_ region, std::string name = "");
private:
	std::string getSkeletonFileName( std::string name );

	bool hasSkeletonCached( std::string name);

	Skeleton_ readSkeleton( std::string file );

	bool cachSkeleton( Skeleton_ skeleton, std::string file );
	
};
S_PTR(RegionSkeletonizer);
/* ������Region���Խ��в�����������ȡ����fit */
class FullSkeletonFitter :
	public SkeletonFitter
{
private:
	Region_ mHumanRegion;
	Skeleton_ mGarmentSkeleton;
	Skeleton_ mHumanSkeleton;
	std::set<size_t> mGarSkeNodes;
	std::set<size_t> mHumanSkeNodes;
public:
	FullSkeletonFitter(void);
	FullSkeletonFitter(Region_ garmentRegion);
	~FullSkeletonFitter(void);

	VertexAlter_ fit(Region_ humanRegion);
	std::string getName();
	static int fitI;
protected:
	/* Override
 	 * ��ȡ��ʼ�����ڵ� */
	size_t getStartSkeletonNode(Region_ region);

	/* Override
	 * ��ȡ��Region�Ĺ��� */
	Skeleton_ getSkeleton(Region_ region);

	/* Override
	 * ��ȡ���ڸ�region�еĹ����ڵ�ļ��� */
	std::set<size_t>& getSkeletonNodesInRegion(Region_ region);

	/* Override */
	void getSortedSkeleton(Region_ region, std::vector<size_t>& sortedSkeNodes,
		MultiNextNodeHandler_ multiNextNodeHandler);
private:
	
	void dumpRegionSkeletonLink(Skeleton_ ske, Region_ region, std::string name);

	/* �������ڵ���±�洢��һ��set�� */
	void addSkeletonNodesToSet(Skeleton_ ske, std::set<size_t>& nodeSet);
};