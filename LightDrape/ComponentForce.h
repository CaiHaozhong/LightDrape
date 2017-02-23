#pragma once
#include "Common.h"
#include "Vec3d.h"
#include <vector>
class Mesh;
S_PTR(Mesh);
/* �ʵ����ܵķ��� */
class ComponentForce
{
public:
	ComponentForce(void);
	~ComponentForce(void);
	virtual Vec3d compute(size_t i, const Mesh_ mesh, const std::vector<Vec3d>& curPositions,
		const std::vector<Vec3d>& curVelocities, const std::vector<double> pointMass) = 0;
};
S_PTR(ComponentForce);

/* ���� */
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

/* ����֮���������������ϵ���ĵ�λ��N/m��ţ��/��
 * ���ɶ���Ϊ F = k * deltaLen;
 */
class StretchForce : public ComponentForce{
private:
	double k;
public:
	StretchForce(){
		k = 75;
	}
	/* kΪ����ϵ������λ��ţ��/�ף�Ĭ����75 */
	StretchForce(double k){
		this->k = k;
	}

	/* kΪ����ϵ������λ��ţ��/�ף�Ĭ����75 */
	void setStiffness(double k){
		this->k = k;
	}

	Vec3d compute(size_t index, const Mesh_ mesh, const std::vector<Vec3d>& curPositions,
		const std::vector<Vec3d>& curVelocities, const std::vector<double> pointMass);
};
S_PTR(StretchForce);