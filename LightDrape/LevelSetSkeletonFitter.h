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
	/* 插值计算该Region中所有顶点的位移
	 * 虚拟一组精确的circle，根据各个顶点的测地值与这些circle的距离来插值
	 * 参数：
	 * alter：【输出参数】该Region的顶点的位移
	 * garMesh：衣服网格模型；
	 * cicleDeltas：每个circle的位移；
	 * circleHeights：每个circle的测地值
	 */
	void computeTranslation( VertexAlter_ alter, WatertightMesh_ garMesh, std::vector<Vec3d>& cicleDeltas, 
		std::vector<double>& circleHeights);

	//void computeRotation( VertexAlter_ alter, WatertightMesh_ garMesh, )
};
S_PTR(LevelSetSkeletonFitter);
