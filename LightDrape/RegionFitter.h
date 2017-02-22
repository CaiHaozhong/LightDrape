#pragma once
#include <string>
#include "Common.h"
#include <vector>
#include "Vec3d.h"
class Region;
S_PTR(Region);
class VertexAlter;
S_PTR(VertexAlter);
class LevelCircle;
S_PTR(LevelCircle);
class Mesh;
S_PTR(Mesh);
class Quaternion;
class RegionFitter
{
protected:
	Region_ mGarmentRegion;
public:
	RegionFitter(void);
	RegionFitter(Region_ garmentRegion);
	~RegionFitter(void);

	/* 匹配两个Region，并返回服装Region的顶点位移 
	 * 形象的理解：将衣服的一个Region穿到人体的一个Region上
	 * 参数：人体的一个Region */
	virtual VertexAlter_ fit(Region_ humanRegion) = 0;

	virtual std::string getName() = 0;

	void setGarmentRegion(Region_ garmentRegion);

protected:
	/* 对一个小区域进行旋转，一般来说小区域是一个圆圈
	 * 输出参数：alter
	 * 输入参数：
	 * circle 小区域，center 该circle的中心，
	 * axis 旋转轴，angle 旋转角（弧度）
	 */
	void rotateCircle(VertexAlter_ alter, const Mesh_ mesh, const std::vector<size_t>& circle, 
		const Vec3d& center, const Vec3d& axis, double angle);

	/* 对一个小区域进行旋转，一般来说小区域是一个圆圈
	 * 输出参数：alter
	 * circle 小区域，center 该circle的中心，
	 * axis 旋转轴，angle 旋转角（弧度）
	 */
	void rotateCircle(VertexAlter_ alter, const Mesh_ mesh, const LevelCircle_ circle, 
		const Vec3d& center,  const Vec3d& axis, double angle);

private:
	/* 对一个点进行旋转
	 * 输出参数：point
	 */
	void rotatePoint(Vec3d& point, const Vec3d& center, Quaternion& quaternion);
};
S_PTR(RegionFitter);
