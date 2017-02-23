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
/* 物理模拟主流程 */
class GarmentPhysicalSimulator
{
public:
	GarmentPhysicalSimulator(void);

	GarmentPhysicalSimulator(Mesh_ garment, Mesh_ human);

	~GarmentPhysicalSimulator(void);

	void setIntegration(Integration_ integration);

	void initWithGarmentAndHuman(Mesh_ garment, Mesh_ human);

	/* 初始化质点的质量和初始的位置、速度 */
	void initPointProperty();

	/* 初始化力的类型 */
	void initForce();

	/* 开始物理模拟 */
	void simulate();

	/* 添加质点所受的力的类型 */
	void addForce(ComponentForce_ force);

	/* 设置时间步长，单位为秒，默认为0.01s */
	void setIntegrateStep(double dt);

private:
	Mesh_ mGarment;

	Mesh_ mHuman;

	Integration_ mIntegration;

	PenetrationResolver_ mPenetrationResolver;

	SimulationFramePool_ mSimulationFramePool;

	/* 质点所受的力 */
	std::vector<ComponentForce_> mForces;

	/* 质点当前的位置 */
	std::vector<Vec3d> mCurPositions;

	/* 质点当前的速度 */
	std::vector<Vec3d> mCurVelocities;

	/* 质点的质量 */
	std::vector<double> mPointMass;

	/* 质点的个数 */
	size_t mPointCount;

	double m_dt;
};

