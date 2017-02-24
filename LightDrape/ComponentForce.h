#pragma once
#include "Common.h"
#include "Vec3d.h"
#include <vector>
class Mesh;
S_PTR(Mesh);
/* �ʵ����ܵķ��� */
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

/* ���� */
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

/* ����֮���������������ϵ���ĵ�λ��N/m��ţ��/��
 * ���ɶ���Ϊ F = k * deltaLen;
 */
class StretchForce : public ComponentForce{
private:
	double k;
public:
	StretchForce(Mesh_ mesh);
	StretchForce();
	/* kΪ����ϵ������λ��ţ��/�ף�Ĭ����75 */
	StretchForce(double k);

	/* kΪ����ϵ������λ��ţ��/�ף�Ĭ����75 */
	void setStiffness(double k);

	Vec3d compute(size_t index, const std::vector<Vec3d>& curPositions,
		const std::vector<Vec3d>& curVelocities, const std::vector<double> pointMass);
};
S_PTR(StretchForce);