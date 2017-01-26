#pragma once
#include "Regionfitter.h"
#include "Vec3d.h"
#include <vector>
#include <tuple>
class WatertightMesh;
S_PTR(WatertightMesh);
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

class SimpleSkeletonFitter :
	public RegionFitter
{
public:
	SimpleSkeletonFitter(void);
	SimpleSkeletonFitter(Region_ garmentRegion);
	~SimpleSkeletonFitter(void);
	VertexAlter_ fit(Region_ humanRegion);
	std::string getName();
private:
	/* �Թ����ڵ㰴�����ӹ�ϵ��������
	 * ������sortedSkeNodes ���
	 * ������multiNextNodeHandler ��һ���ڵ����һ���ڵ��ж����ѡʱ�����������
	 * ����ѡ�ĸ�
	 */
	void getSortedSkeleton(Region_ region, std::vector<size_t>& sortedSkeNodes, 
		MultiNextNodeHandler_ multiNextNodeHandler);

	/* ����������������õ��·�����Ĺ����ڵ��λ���� 
	 * ���� garSkeTrans Ϊ���
	 * garSkeTrans�еĹ����ڵ㰴�����ӹ�ϵ�ӿ�ʼ�ڵ㰴˳��洢
	 */
	void fitSkeleton(std::vector< std::pair<size_t, Vec3d> >& garSkeTrans, Region_ humanRegion);

	/* ��Ȩ��������ڵ��Ӧ�����񶥵��λ��
	 * ÿ�����񶥵������Ӧ�Ĺ����ڵ㼰�������������ڵ��Ӱ�죬�߽���Ϊһ��
	 * �����񶥵�������ڵ�ľ����Ȩƽ��
	 */
	VertexAlter_ computeVertexAlters(WatertightMesh_ mesh, std::vector< std::pair<size_t, Vec3d> >& garSkeTrans);
};
S_PTR(SimpleSkeletonFitter);
