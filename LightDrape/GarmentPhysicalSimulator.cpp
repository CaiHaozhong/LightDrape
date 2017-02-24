#include "GarmentPhysicalSimulator.h"
#include "Mesh.h"
#include "PenetrationResolver.h"
#include "Integration.h"
#include "MeshFramePool.h"
#include "ComponentForce.h"
#include "SimpleIntegration.h"
#include "GarmentSimulationCallBack.h"
#include "VerletIntegration.h"

GarmentPhysicalSimulator::GarmentPhysicalSimulator(void)
{
	mIntegration = nullptr;
	mPenetrationResolver = nullptr;
	mMeshFramePool = nullptr;
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
//	mIntegration = std::make_shared<VerletIntegration>(garment->n_vertices());
	mIntegration = std::make_shared<SimpleIntegration>();
	mPenetrationResolver = smartNew(PenetrationResolver);
	mPenetrationResolver->setRigidMesh(human);
	mPenetrationResolver->setAllowDis(0.5);
	std::vector< std::vector<size_t> > garAdjList;
	int cur = 0;
	garAdjList.resize(garment->n_vertices(), std::vector<size_t>());
	for(Mesh::VertexIter it = garment->vertices_begin(); it != garment->vertices_end(); it++){
		for(Mesh::ConstVertexVertexIter vvIt = garment->cvv_begin(*it); vvIt.is_valid(); vvIt++){
			garAdjList[cur].push_back(vvIt->idx());
		}
		cur += 1;
	}
	mPenetrationResolver->setAdjList(garAdjList);
	mMeshFramePool = smartNew(MeshFramePool);
	mMeshFramePool->storeFrame(garment);
	setIntegrateStep(0.0001);
	mAccumulateTimeInterFrame = 0;
	mCurTime = 0;
	mSimulateLen = 5;
	initPointProperty();
	initForce();
}

void GarmentPhysicalSimulator::initPointProperty()
{
	mPointCount = mGarment->n_vertices();
	mCurPositions.resize(mPointCount);

	/* 初始速度为0 */
	mCurVelocities.resize(mPointCount, Vec3d(0, 0, 0));	
	/* 初始质量为1 */
	mPointMass.resize(mPointCount, 1);

	/* 设置初始位置 */
	for(Mesh::VertexIter it = mGarment->vertices_begin(); it != mGarment->vertices_end(); it++){
		mCurPositions[it->idx()] = mGarment->point(*it);
	}
}

void GarmentPhysicalSimulator::initForce()
{
	mForces.push_back(smartNew(GravityForce));
	mForces.push_back(std::make_shared<StretchForce>(mGarment));
}

void GarmentPhysicalSimulator::addForce( ComponentForce_ force )
{
	mForces.push_back(force);
}

void GarmentPhysicalSimulator::simulate()
{
	if(mGarment == nullptr || mHuman == nullptr){
		PRINT_ERROR("In GarmentPhysicalSimulator::simulate(), should initWithGarmentAndHuman first.");
		return;
	}
	onBegin();
	while(true){
		mAccumulateTimeInterFrame += m_dt;
		mCurTime += m_dt;
		/* m_dt时间步长后的状态 */
		std::vector<Vec3d> nextPositions(mPointCount);
		std::vector<Vec3d> nextVelocities(mPointCount);

// 		/* 当前每个质点所受的合力 */
// 		std::vector<Vec3d> curTotalForces(mPointCount);

// 		/* 计算当前每个质点所受的合力 */
// 		for(size_t i = 0; i < mPointCount; i++){
// 			Vec3d totalForce(0, 0, 0);
// 			for(auto it = mForces.begin(); it != mForces.end(); it++){
// 				totalForce += (*it)->compute(i, mGarment, mCurPositions, mCurVelocities, mPointMass);			
// 			}
// 			curTotalForces[i] = totalForce;
// 		}

// 		/* 计算m_dt时间步长后每个质点的位置和速度 */
// 		for(size_t i = 0; i < mPointCount; i++){
// 			std::pair<Vec3d, Vec3d> nextPosVelo
// 				= mIntegration->integrate2(
// 				mCurPositions[i],
// 				mCurVelocities[i],
// 				curTotalForces[i],
// 				mPointMass[i],
// 				m_dt);
// 			nextPositions[i] = nextPosVelo.first;
// 			nextVelocities[i] = nextPosVelo.second;
// 		}

		/* 计算m_dt时间步长后每个质点的位置和速度 */
		for(size_t i = 0; i < mPointCount; i++){
			std::pair<Vec3d, Vec3d> nextPosVelo = mIntegration->integrate(
				i, mCurPositions, mCurVelocities, mPointMass, mForces, m_dt);

			nextPositions[i] = nextPosVelo.first;
			nextVelocities[i] = nextPosVelo.second;
		}

		/* 解决衣服与人体的穿透 */
		std::vector<size_t> penetrationVers;	
		mPenetrationResolver->resolve(nextPositions, penetrationVers);
		for(size_t i = 0; i < penetrationVers.size(); i++){
			mCurVelocities[i] = - mCurVelocities[i];
		}
		while(mPenetrationResolver->resolve(nextPositions, penetrationVers)){}


		/* 存储m_dt时间步长后的状态 */
		mCurPositions = nextPositions;
		mCurVelocities = nextVelocities;

		/* 得到了一帧 */
		if(mAccumulateTimeInterFrame >= 1.0/mMeshFramePool->getFPS()){
			mMeshFramePool->storeFrame(mCurPositions);
			mAccumulateTimeInterFrame = 0;
			onFrame(std::make_shared<MeshFrame>(mCurPositions));
		}

		/* 结束模拟 */
		if(mCurTime >= mSimulateLen){
			break;
		}
	}
	onEnd(mMeshFramePool);
}

void GarmentPhysicalSimulator::setIntegrateStep( double dt )
{
	m_dt = dt;
}

void GarmentPhysicalSimulator::addGarmentSimulationCallBack( GarmentSimulationCallBack_ callback )
{
	mGarmentSimulationCallBacks.push_back(callback);
}

void GarmentPhysicalSimulator::onBegin()
{
	for(auto it = mGarmentSimulationCallBacks.begin(); it != mGarmentSimulationCallBacks.end(); it++){
		(*it)->onSimulateBegin();
	}
}

void GarmentPhysicalSimulator::onFrame( MeshFrame_ frame )
{
	for(auto it = mGarmentSimulationCallBacks.begin(); it != mGarmentSimulationCallBacks.end(); it++){
		(*it)->onFrame(frame);
	}
}

void GarmentPhysicalSimulator::onEnd( MeshFramePool_ meshFramePool )
{
	for(auto it = mGarmentSimulationCallBacks.begin(); it != mGarmentSimulationCallBacks.end(); it++){
		(*it)->onSimulateEnd(meshFramePool);
	}
}
