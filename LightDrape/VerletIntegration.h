#pragma once
#include <vector>
#include "Vec3d.h"
#include "integration.h"
class VerletIntegration :
	public Integration
{
private:
	std::vector<Vec3d> mLastPostion;	
	size_t mHasLasPositionPointCount;
public:
	VerletIntegration(size_t pointSize);
	VerletIntegration();
	~VerletIntegration(void);
	
	/* 设置顶点的个数 */
	void setPointSize(size_t pointSize);

	/* 积分求解一个时间步长后的位置和速度 
	 * 参数：
	 * x_t：当前的位置
	 * v_t：当前的速度
	 * f_t：当前的合力
	 * mass：质点的质量
	 * dt：时间步长
	 * 返回值：
	 * pair.first：dt时间后的位置
	 * pair.second：dt时间后的速度
	 */
	std::pair<Vec3d, Vec3d> integrate2(const Vec3d& x_t, const Vec3d& v_t, const Vec3d& f_t, const double mass, double dt);


	std::pair<Vec3d, Vec3d> integrate(size_t i, const std::vector<Vec3d>& curPositions,
		const std::vector<Vec3d>& curVelocities, const std::vector<double>& pointMass,
		const std::vector<ComponentForce_> forces, double dt);
};