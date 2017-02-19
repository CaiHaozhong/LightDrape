#pragma once
#include "regionfitter.h"
class Quaternion;
S_PTR(Quaternion);
class RegionSkeleton;
S_PTR(RegionSkeleton);
class WatertightMesh;
S_PTR(WatertightMesh);
class RegionSkeletonFitter :
	public RegionFitter
{
private:
	Region_ mHumanRegion;
public:
	RegionSkeletonFitter(void);
	RegionSkeletonFitter(Region_ garmentRegion);
	~RegionSkeletonFitter(void);


	/* ƥ������Region�������ط�װRegion�Ķ���λ�� 
	 * �������⣺���·���һ��Region���������һ��Region��
	 * �����������һ��Region */
	VertexAlter_ fit(Region_ humanRegion);

	std::string getName();

	void fitSkeleton(std::vector< std::pair<size_t, Vec3d> >& garSkeTrans, 
		std::vector< std::pair<size_t, std::pair<Vec3d, double> > >& garSkeRotate);
private:
	/* ��Ȩ��������ڵ��Ӧ�����񶥵��λ��
	 * ÿ�����񶥵������Ӧ�Ĺ����ڵ㼰�������������ڵ��Ӱ�죬�߽���Ϊһ��
	 * �����񶥵�������ڵ�ľ����Ȩƽ��
	 */
	void computeTranslation(VertexAlter_ ret,
		std::vector< std::pair<size_t, Vec3d> >& garSkeTrans);

	/* �Թ����ڵ��Ӧ�����񶥵������ת */
	void computeRotation(VertexAlter_ ret,
		std::vector< std::pair<size_t, std::pair<Vec3d, double> > >& garSkeRotate);
};

