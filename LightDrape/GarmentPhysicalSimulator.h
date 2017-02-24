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
class MeshFramePool;
S_PTR(MeshFramePool);
class MeshFrame;
S_PTR(MeshFrame);
class ComponentForce;
S_PTR(ComponentForce);
class GarmentSimulationCallBack;
S_PTR(GarmentSimulationCallBack);
/* ����ģ�������� */
class GarmentPhysicalSimulator
{
public:
	GarmentPhysicalSimulator(void);

	GarmentPhysicalSimulator(Mesh_ garment, Mesh_ human);

	~GarmentPhysicalSimulator(void);

	void setIntegration(Integration_ integration);

	void initWithGarmentAndHuman(Mesh_ garment, Mesh_ human);

	/* ��ʼ����ģ�� */
	void simulate();

	/* ����ʵ����ܵ��������� */
	void addForce(ComponentForce_ force);

	/* ����ʱ�䲽������λΪ�룬Ĭ��Ϊ0.01s */
	void setIntegrateStep(double dt);

	/* ģ�����ʱ�䣬��λΪ�� */
	double setSimulateLen() const { return mSimulateLen; }

	/* ģ�����ʱ�䣬��λΪ�� */
	void getSimulateLen(double val) { mSimulateLen = val; }

	void addGarmentSimulationCallBack(GarmentSimulationCallBack_ callback);

private:
	/* ��ʼ���ʵ�������ͳ�ʼ��λ�á��ٶ� */
	void initPointProperty();

	/* ��ʼ���������� */
	void initForce();

	void onBegin();
	void onFrame(MeshFrame_ frame);
	void onEnd(MeshFramePool_ meshFramePool);
private:
	Mesh_ mGarment;

	Mesh_ mHuman;

	Integration_ mIntegration;

	PenetrationResolver_ mPenetrationResolver;

	MeshFramePool_ mMeshFramePool;

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

	/* ֡��ʱ���ۻ�����λΪ�� */
	double mAccumulateTimeInterFrame;

	/* ��ǰ��ʱ�䣬��λΪ�� */
	double mCurTime;

	/* ģ�����ʱ�䣬��λΪ�� */
	double mSimulateLen;

	/* ģ��Ļص� */
	std::vector<GarmentSimulationCallBack_> mGarmentSimulationCallBacks;

};
S_PTR(GarmentPhysicalSimulator);