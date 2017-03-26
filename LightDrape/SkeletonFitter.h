#pragma once
#include "regionfitter.h"
#include <set>
class WatertightMesh;
S_PTR(WatertightMesh);
class Skeleton;
S_PTR(Skeleton);
class VertexAlter;
S_PTR(VertexAlter);
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

class ChooseRadomBranch : public MultiNextNodeHandler{
public:
	ChooseRadomBranch(Skeleton_ skeleton):MultiNextNodeHandler(skeleton){}

	size_t decide(size_t prev, std::vector<size_t>& nextNodes);
private:
};
S_PTR(ChooseRadomBranch);
class SkeletonFitter :
	public RegionFitter
{
public:
	SkeletonFitter(void);
	SkeletonFitter(Region_ garmentRegion);
	~SkeletonFitter(void);

protected:
	VertexAlter_ fit(Region_ humanRegion);

	/* ����������������õ��·�����Ĺ����ڵ��λ���� 
	 * ���� garSkeTrans Ϊ���
	 * garSkeTrans�еĹ����ڵ㰴�����ӹ�ϵ�ӿ�ʼ�ڵ㰴˳��洢
	 */
	void fitSkeleton(std::vector< std::pair<size_t, Vec3d> >& garSkeTrans, 
		std::vector< std::pair<size_t, std::pair<Vec3d, double> > >& garSkeRotate,
		Region_ humanRegion);

	/* ��Ȩ��������ڵ��Ӧ�����񶥵��λ��
	 * ÿ�����񶥵������Ӧ�Ĺ����ڵ㼰�������������ڵ��Ӱ�죬�߽���Ϊһ��
	 * �����񶥵�������ڵ�ľ����Ȩƽ��
	 */
	void computeTranslation(VertexAlter_ ret, Region_ region, std::vector< std::pair<size_t, Vec3d> >& garSkeTrans);

	/* �Թ����ڵ��Ӧ�����񶥵������ת */
	void computeRotation(VertexAlter_ ret, Region_ region, 
		std::vector< std::pair<size_t, std::pair<Vec3d, double> > >& garSkeRotate);

	/* �Թ����ڵ㰴�����ӹ�ϵ��������
	 * ������sortedSkeNodes ���
	 * ������multiNextNodeHandler ��һ���ڵ����һ���ڵ��ж����ѡʱ�����������
	 * ����ѡ�ĸ�
	 */
	virtual void getSortedSkeleton(Region_ region, std::vector<size_t>& sortedSkeNodes,
		MultiNextNodeHandler_ multiNextNodeHandler);

	/* ��ȡ��ʼ�����ڵ� */
	virtual size_t getStartSkeletonNode(Region_ region) = 0;

	/* ��ȡ��Region�Ĺ��� */
	virtual Skeleton_ getSkeleton(Region_ region) = 0;

	/* ��ȡ���ڸ�region�еĹ����ڵ�ļ��� */
	virtual std::set<size_t>& getSkeletonNodesInRegion(Region_ region) = 0;

	void dumpDeformSkeleton(std::vector< std::pair<size_t, Vec3d> >& garSkeTrans);
};

