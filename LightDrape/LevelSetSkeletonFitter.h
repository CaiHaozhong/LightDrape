#pragma once
#include "Regionfitter.h"
#include <vector>
#include "Vec3d.h"
class LevelCircle;
S_PTR(LevelCircle);
class WatertightMesh;
S_PTR(WatertightMesh);
class VertexAlter;
S_PTR(VertexAlter);
class LevelSetSkeletonFitter :
	public RegionFitter
{
private:
	Region_ mHumanRegion;
public:
	LevelSetSkeletonFitter(void);
	~LevelSetSkeletonFitter(void);
	LevelSetSkeletonFitter(Region_ garmentRegion);
	VertexAlter_ fit(Region_ humanRegion);	
	std::string getName();
private:
	/* ��ֵ�����Region�����ж����λ��
	 * ����һ�龫ȷ��circle�����ݸ�������Ĳ��ֵ����Щcircle�ľ�������ֵ
	 * ������
	 * alter���������������Region�Ķ����λ��
	 * garMesh���·�����ģ�ͣ�
	 * cicleDeltas��ÿ��circle��λ�ƣ�
	 * circleHeights��ÿ��circle�Ĳ��ֵ
	 */
	void computeTranslation( VertexAlter_ alter, WatertightMesh_ garMesh, std::vector<Vec3d>& cicleDeltas, 
		std::vector<double>& circleHeights);

	//void computeRotation( VertexAlter_ alter, WatertightMesh_ garMesh, )
};
S_PTR(LevelSetSkeletonFitter);
