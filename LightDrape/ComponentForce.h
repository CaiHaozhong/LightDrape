#pragma once
#include "Common.h"
#include "Vec3d.h"
#include <vector>
class Mesh;
S_PTR(Mesh);
/* 质点所受的分力 */
class ComponentForce
{
protected:
	Mesh_ mMesh;
public:
	ComponentForce(void);
	~ComponentForce(void);
	ComponentForce(Mesh_ mesh);
	Mesh_ setMesh() const;
	void getMesh(Mesh_ val);
	virtual Vec3d compute(size_t i, const std::vector<Vec3d>& curPositions,
		const std::vector<Vec3d>& curVelocities, const std::vector<double> pointMass) = 0;
};
S_PTR(ComponentForce);

/* 重力 */
class GravityForce : public ComponentForce{
private:
	Vec3d g;
public:
	GravityForce(Mesh_ mesh);
	GravityForce();
	Vec3d compute(size_t i, const std::vector<Vec3d>& curPositions,
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
	StretchForce(Mesh_ mesh);
	StretchForce();
	/* k为劲度系数，单位是牛顿/米，默认是75 */
	StretchForce(double k);

	/* k为劲度系数，单位是牛顿/米，默认是75 */
	void setStiffness(double k);

	Vec3d compute(size_t index, const std::vector<Vec3d>& curPositions,
		const std::vector<Vec3d>& curVelocities, const std::vector<double> pointMass);
};
S_PTR(StretchForce);