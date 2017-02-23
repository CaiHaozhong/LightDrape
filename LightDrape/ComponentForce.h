#pragma once
#include "Common.h"
#include "Vec3d.h"
#include <vector>
class Mesh;
S_PTR(Mesh);
/* 质点所受的分力 */
class ComponentForce
{
public:
	ComponentForce(void);
	~ComponentForce(void);
	virtual Vec3d compute(size_t i, const Mesh_ mesh, const std::vector<Vec3d>& curPositions,
		const std::vector<Vec3d>& curVelocities, const std::vector<double> pointMass) = 0;
};
S_PTR(ComponentForce);

/* 重力 */
class GravityForce : public ComponentForce{
private:
	Vec3d g;
public:
	GravityForce(){
		g = Vec3d(0, 9.8, 0);
	}
	Vec3d compute(size_t i, const Mesh_ mesh, const std::vector<Vec3d>& curPositions,
		const std::vector<Vec3d>& curVelocities, const std::vector<double> pointMass);

};
S_PTR(GravityForce);

/* 弹簧之间的拉伸力，劲度系数的单位是N/m，牛顿/米
 * 胡可定律为 F = k * deltaLen;
 */
class StretchForce : public ComponentForce{
private:
	double k;
public:
	StretchForce(){
		k = 75;
	}
	/* k为劲度系数，单位是牛顿/米，默认是75 */
	StretchForce(double k){
		this->k = k;
	}

	/* k为劲度系数，单位是牛顿/米，默认是75 */
	void setStiffness(double k){
		this->k = k;
	}

	Vec3d compute(size_t index, const Mesh_ mesh, const std::vector<Vec3d>& curPositions,
		const std::vector<Vec3d>& curVelocities, const std::vector<double> pointMass);
};
S_PTR(StretchForce);