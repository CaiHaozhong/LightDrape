#include "GarmentPhysicalSimulator.h"
#include "Mesh.h"
#include "PenetrationResolver.h"
#include "Integration.h"
#include "SimulationFramePool.h"
#include "ComponentForce.h"


GarmentPhysicalSimulator::GarmentPhysicalSimulator(void)
{
	mIntegration = nullptr;
	mPenetrationResolver = nullptr;
	mSimulationFramePool = smartNew(SimulationFramePool);
}


GarmentPhysicalSimulator::~GarmentPhysicalSimulator(void)
{
}

GarmentPhysicalSimulator::GarmentPhysicalSimulator( Mesh_ garment, Mesh_ human )
{
	initWithGarmentAndHuman(garment, human);
}

void GarmentPhysicalSimulator::setIntegration( Integration_ integration )
{
	mIntegration = integration;
}

void GarmentPhysicalSimulator::initWithGarmentAndHuman( Mesh_ garment, Mesh_ human )
{
	mGarment = garment;
	mHuman = human;
	mPenetrationResolver = smartNew(PenetrationResolver);
	mSimulationFramePool = smartNew(SimulationFramePool);
	setIntegrateStep(0.01);
	initPointProperty();
}

void GarmentPhysicalSimulator::initPointProperty()
{
	mPointCount = mGarment->n_vertices();
	mCurPositions.resize(mPointCount);

	/* ��ʼ�ٶ�Ϊ0 */
	mCurVelocities.resize(mPointCount, Vec3d(0, 0, 0));	
	/* ��ʼ����Ϊ1 */
	mPointMass.resize(mPointCount, 1);

	/* ���ó�ʼλ�� */
	for(Mesh::VertexIter it = mGarment->vertices_begin(); it != mGarment->vertices_end(); it++){
		mCurPositions[it->idx()] = mGarment->point(*it);
	}
}

void GarmentPhysicalSimulator::initForce()
{
	mForces.push_back(smartNew(GravityForce));
	mForces.push_back(smartNew(StretchForce));
}

void GarmentPhysicalSimulator::addForce( ComponentForce_ force )
{
	mForces.push_back(force);
}

void GarmentPhysicalSimulator::simulate()
{
	/* m_dtʱ�䲽�����״̬ */
	std::vector<Vec3d> nextPositions(mPointCount);
	std::vector<Vec3d> nextVelocities(mPointCount);

	/* ��ǰÿ���ʵ����ܵĺ��� */
	std::vector<Vec3d> curTotalForces(mPointCount);

	/* ���㵱ǰÿ���ʵ����ܵĺ��� */
	for(size_t i = 0; i < mPointCount; i++){
		Vec3d totalForce(0, 0, 0);
		for(auto it = mForces.begin(); it != mForces.end(); it++){
			totalForce += (*it)->compute(i, mGarment, mCurPositions, mCurVelocities, mPointMass);			
		}
		curTotalForces[i] = totalForce;
	}

	/* ����m_dtʱ�䲽����ÿ���ʵ��λ�ú��ٶ� */
	for(size_t i = 0; i < mPointCount; i++){
		std::pair<Vec3d, Vec3d> nextPosVelo
			= mIntegration->integrate(mCurPositions[i],
			mCurVelocities[i],
			curTotalForces[i],
			m_dt
			);
		nextPositions[i] = nextPosVelo.first;
		nextVelocities[i] = nextPosVelo.second;
	}

	/* �洢m_dtʱ�䲽�����״̬ */
	mCurPositions = nextPositions;
	mCurVelocities = nextVelocities;
}

void GarmentPhysicalSimulator::setIntegrateStep( double dt )
{
	m_dt = dt;
}
