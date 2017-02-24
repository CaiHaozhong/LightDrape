#pragma once
#include "integration.h"
/* 简单的积分，显式欧拉积分法 */
class SimpleIntegration :
	public Integration
{
public:
	SimpleIntegration(void);
	~SimpleIntegration(void);

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

