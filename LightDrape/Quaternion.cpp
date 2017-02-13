#include "Quaternion.h"


Quaternion::Quaternion(void)
{
}

Quaternion::Quaternion( const Vec3d& axis, double angle )
{
	mAxis = axis;
	mAxis.normalize_cond();
	mAngle = angle;
	mCosA = cos(mAngle);
	mSinA = sin(mAngle);
}


Quaternion::~Quaternion(void)
{
}

Vec3d Quaternion::constRotate( const Vec3d& point )
{
	return (point - mAxis * (mAxis | point)) * mCosA
		+ (mAxis % point) * mSinA 
		+ mAxis * (mAxis | point);
}

void Quaternion::rotate( Vec3d& point )
{
	point = constRotate(point);
}
