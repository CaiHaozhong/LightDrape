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

	/* ƥ������Region�������ط�װRegion�Ķ���λ�� 
	 * �������⣺���·���һ��Region���������һ��Region��
	 * �����������һ��Region */
	virtual VertexAlter_ fit(Region_ humanRegion) = 0;

	virtual std::string getName() = 0;

	void setGarmentRegion(Region_ garmentRegion);

protected:
	/* ��һ��С���������ת��һ����˵С������һ��ԲȦ
	 * ���������alter
	 * ���������
	 * circle С����center ��circle�����ģ�
	 * axis ��ת�ᣬangle ��ת�ǣ����ȣ�
	 */
	void rotateCircle(VertexAlter_ alter, const Mesh_ mesh, const std::vector<size_t>& circle, 
		const Vec3d& center, const Vec3d& axis, double angle);

	/* ��һ��С���������ת��һ����˵С������һ��ԲȦ
	 * ���������alter
	 * circle С����center ��circle�����ģ�
	 * axis ��ת�ᣬangle ��ת�ǣ����ȣ�
	 */
	void rotateCircle(VertexAlter_ alter, const Mesh_ mesh, const LevelCircle_ circle, 
		const Vec3d& center,  const Vec3d& axis, double angle);

private:
	/* ��һ���������ת
	 * ���������point
	 */
	void rotatePoint(Vec3d& point, const Vec3d& center, Quaternion& quaternion);
};
S_PTR(RegionFitter);
