#pragma once
#include "Vec3d.h"
/* 四元数，实现绕任意轴旋转
 * 参照：https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Proof_of_the_quaternion_rotation_identity
 */
class Quaternion
{
private:
	Vec3d mAxis;

	/* 弧度 */
	double mAngle;

	double mCosA, mSinA;
public:
	Quaternion(void);

	Quaternion(const Vec3d& axis, double angle);

	~Quaternion(void);

	void rotate(Vec3d& point);

	Vec3d constRotate(const Vec3d& point);
};

