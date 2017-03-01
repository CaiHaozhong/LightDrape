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
	virtual void compute(
		const std::vector<Vec3d>& curPositions,
		const std::vector<Vec3d>& curVelocities,
		const std::vector<double>& pointMass,
		std::vector<Vec3d>& forces) = 0;
};
S_PTR(ComponentForce);

/* ���� */
class GravityForce : public ComponentForce{
private:
	Vec3d g;
public:
	GravityForce(Mesh_ mesh);
	GravityForce();
	void compute(
		const std::vector<Vec3d>& curPositions,
		const std::vector<Vec3d>& curVelocities,
		const std::vector<double>& pointMass,
		std::vector<Vec3d>& forces);

};
S_PTR(GravityForce);

/* ����֮���������������ϵ���ĵ�λ��N/m��ţ��/��
 * ���ɶ���Ϊ F = k * deltaLen;
 */
class StretchForce : public ComponentForce{
public:
	struct Spring{
		size_t p1, p2;
		double rest_length;
		double Ks, Kd;
	};
private:
	double k;
	std::vector<Spring> mSprings;
public:
	StretchForce(Mesh_ mesh);
	StretchForce();
	/* kΪ����ϵ������λ��ţ��/�ף�Ĭ����75 */
	StretchForce(double k);

	void initSpring(Mesh_ mesh);

	/* kΪ����ϵ������λ��ţ��/�ף�Ĭ����75 */
	void setStiffness(double k);

	void compute(
		const std::vector<Vec3d>& curPositions,
		const std::vector<Vec3d>& curVelocities,
		const std::vector<double>& pointMass,
		std::vector<Vec3d>& forces);
};
S_PTR(StretchForce);

class DampForce :
	public ComponentForce
{
public:
	DampForce(void);
	~DampForce(void);
	DampForce(double damping);
	void compute(
		const std::vector<Vec3d>& curPositions,
		const std::vector<Vec3d>& curVelocities,
		const std::vector<double>& pointMass,
		std::vector<Vec3d>& forces);
private:
	double mDamping;
};
S_PTR(DampForce);

class BendForce :
	public ComponentForce{
public:
	struct FaceAngleSpring{
		size_t ep1, ep2; // ���ߵ�������
		size_t fp1, fp2; // �����ߵ�������
		double rest_angle;
		double Ks, Kd;
	};
private:
	std::vector<FaceAngleSpring> mSprings;	
	double mPi;
public:
	BendForce();
	BendForce(Mesh_ mesh);
	void initSpring(Mesh_ mesh);
	void compute(
		const std::vector<Vec3d>& curPositions,
		const std::vector<Vec3d>& curVelocities,
		const std::vector<double>& pointMass,
		std::vector<Vec3d>& forces);
};
S_PTR(BendForce);