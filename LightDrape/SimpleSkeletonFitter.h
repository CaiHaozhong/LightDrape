#pragma once
#include "SkeletonFitter.h"
#include "Vec3d.h"
#include <vector>
#include <tuple>

class SimpleSkeletonFitter :
	public SkeletonFitter
{
public:
	SimpleSkeletonFitter(void);
	SimpleSkeletonFitter(Region_ garmentRegion);
	~SimpleSkeletonFitter(void);
	VertexAlter_ fit(Region_ humanRegion);
	std::string getName();

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

};
S_PTR(SimpleSkeletonFitter);
