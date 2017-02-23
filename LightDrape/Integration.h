#pragma once
#include "Common.h"
#include "Vec3d.h"
#include <tuple>
/* 积分求解法 */
class Integration
{
public:
	Integration(void);
	~Integration(void);

	/* 积分求解一个时间步长后的位置和速度 
	 * 参数：
	 * x_t：当前的位置
	 * v_t：当前的速度
	 * f_t：当前的合力 
	 * dt：时间步长
	 * 返回值：
	 * pair.first：dt时间后的位置
	 * pair.second：dt时间后的速度
	 */
	virtual std::pair<Vec3d, Vec3d> integrate(const Vec3d& x_t, const Vec3d& v_t, const Vec3d& f_t, double dt) = 0;
};
S_PTR(Integration);

