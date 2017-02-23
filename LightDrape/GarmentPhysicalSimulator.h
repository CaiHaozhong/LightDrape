#pragma once
#include "Common.h"
#include <vector>
#include "Vec3d.h"
class Mesh;
S_PTR(Mesh);
class Integration;
S_PTR(Integration);
class PenetrationResolver;
S_PTR(PenetrationResolver);
class SimulationFramePool;
S_PTR(SimulationFramePool);
class ComponentForce;
S_PTR(ComponentForce);
/* ����ģ�������� */
class GarmentPhysicalSimulator
{
public:
	GarmentPhysicalSimulator(void);

	GarmentPhysicalSimulator(Mesh_ garment, Mesh_ human);

	~GarmentPhysicalSimulator(void);

	void setIntegration(Integration_ integration);

	void initWithGarmentAndHuman(Mesh_ garment, Mesh_ human);

	/* ��ʼ���ʵ�������ͳ�ʼ��λ�á��ٶ� */
	void initPointProperty();

	/* ��ʼ���������� */
	void initForce();

	/* ��ʼ����ģ�� */
	void simulate();

	/* ����ʵ����ܵ��������� */
	void addForce(ComponentForce_ force);

	/* ����ʱ�䲽������λΪ�룬Ĭ��Ϊ0.01s */
	void setIntegrateStep(double dt);

private:
	Mesh_ mGarment;

	Mesh_ mHuman;

	Integration_ mIntegration;

	PenetrationResolver_ mPenetrationResolver;

	SimulationFramePool_ mSimulationFramePool;

	/* �ʵ����ܵ��� */
	std::vector<ComponentForce_> mForces;

	/* �ʵ㵱ǰ��λ�� */
	std::vector<Vec3d> mCurPositions;

	/* �ʵ㵱ǰ���ٶ� */
	std::vector<Vec3d> mCurVelocities;

	/* �ʵ������ */
	std::vector<double> mPointMass;

	/* �ʵ�ĸ��� */
	size_t mPointCount;

	double m_dt;
};

