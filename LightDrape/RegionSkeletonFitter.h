#pragma once
#include "regionfitter.h"
class Quaternion;
S_PTR(Quaternion);
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
		std::vector< std::pair<size_t, Quaternion_ > >& garSkeRotate);
};

