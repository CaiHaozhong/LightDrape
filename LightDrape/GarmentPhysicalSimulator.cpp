#include "GarmentPhysicalSimulator.h"
#include "Mesh.h"
#include "PenetrationResolver.h"
#include "Integration.h"
#include "MeshFramePool.h"
#include "ComponentForce.h"
#include "SimpleIntegration.h"
#include "GarmentSimulationCallBack.h"
#include "VerletIntegration.h"
#include "KNNSHelper.h"
#include <unordered_set>
#include "AABBTree.h"
#include "Quaternion.h"
GarmentPhysicalSimulator::GarmentPhysicalSimulator(void)
{
	mIntegration = nullptr;
/*	mPenetrationResolver = nullptr;*/
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
	mIntegration = std::make_shared<SimpleIntegration>();
// 	mPenetrationResolver = smartNew(PenetrationResolver);
// 	mPenetrationResolver->setRigidMesh(human);
// 	mPenetrationResolver->setAllowDis(0.5);
// 	std::vector< std::vector<size_t> > garAdjList;
// 	int cur = 0;
// 	garAdjList.resize(garment->n_vertices(), std::vector<size_t>());
// 	for(Mesh::VertexIter it = garment->vertices_begin(); it != garment->vertices_end(); it++){
// 		for(Mesh::ConstVertexVertexIter vvIt = garment->cvv_begin(*it); vvIt.is_valid(); vvIt++){
// 			garAdjList[cur].push_back(vvIt->idx());
// 		}
// 		cur += 1;
// 	}
// 	mPenetrationResolver->setAdjList(garAdjList);
//	mHumanCollisionModel = std::make_shared<COLDET::CollisionModel3DImpl>(true);
// 	for(auto it = mHuman->faces_begin(); it != mHuman->faces_end(); it++){
// 		float v[3][3];
// 		int cur = 0;
// 		for(auto f_v = mHuman->fv_begin(*it); f_v.is_valid(); f_v++){
// 			const Vec3d& p = mHuman->point(*f_v);
// 			v[cur][0] = (float)(p.values_[0]);
// 			v[cur][1] = (float)(p.values_[1]);
// 			v[cur][2] = (float)(p.values_[2]);
// 			cur += 1;
// 		}
// 		mHumanCollisionModel->addTriangle(v[0], v[1], v[2]);
// 	}
// 	mHumanCollisionModel->finalize();
	mAABBTree = smartNew(AABBTree);
	mAABBTree->initWithMesh(human);

	mMeshFramePool = smartNew(MeshFramePool);
	mMeshFramePool->storeFrame(garment);
	setIntegrateStep(1 / 60.0);
	mAccumulateTimeInterFrame = 0;
	mCurTime = 0;
	mSimulateLen = 20;
	initPointProperty();
	initForce();
//	computeCollisionParts();
}

void GarmentPhysicalSimulator::initPointProperty()
{
	mPointCount = mGarment->n_vertices();
	mCurPositions.resize(mPointCount);

	/* 初始速度为0 */
	mCurVelocities.resize(mPointCount, Vec3d(0, 0, 0));	
	/* 初始质量为1 */
	mPointMass.resize(mPointCount, 0.5);

	/* 设置初始位置 */
	for(Mesh::VertexIter it = mGarment->vertices_begin(); it != mGarment->vertices_end(); it++){
		mCurPositions[it->idx()] = mGarment->point(*it);
	}
}

void GarmentPhysicalSimulator::initForce()
{
	mForces.push_back(smartNew(GravityForce));
	mForces.push_back(std::make_shared<StretchForce>(mGarment));
	mForces.push_back(std::make_shared<DampForce>());
	mForces.push_back(std::make_shared<BendForce>(mGarment));
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
// 		std::vector<Vec3d> nextPositions(mPointCount);
// 		std::vector<Vec3d> nextVelocities(mPointCount);

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
// 		for(size_t i = 0; i < mPointCount; i++){
// 			std::pair<Vec3d, Vec3d> nextPosVelo = mIntegration->integrate(
// 				i, mCurPositions, mCurVelocities, mPointMass, mForces, m_dt);
// 
// 			nextPositions[i] = nextPosVelo.first;
// 			nextVelocities[i] = nextPosVelo.second;
// 		}

// 		Vec3d buffer[50];
// 		for(size_t k = 0; k < 50; k++){
// 			buffer[k] = mCurPositions[k];
// 		}

		/* 更新m_dt时间步长后每个质点的位置和速度 */
		mIntegration->integrate(mCurPositions, mCurVelocities, mPointMass, mGarment, mForces, m_dt);

// 		for(size_t k = 0; k < 50; k++){
// 			mCurPositions[k] = buffer[k];
// 		}
		

// 		/* 解决衣服与人体的穿透 */
//   		std::vector<size_t> penetrationVers;	
// 		mPenetrationResolver->resolve(mCurPositions, penetrationVers);
// 		for(size_t i = 0; i < penetrationVers.size(); i++){
// 			mCurVelocities[i] = Vec3d(0, 0, 0);
// 		}
// 		while(mPenetrationResolver->resolve(mCurPositions, penetrationVers)){}

		

// 		for(size_t i = 0; i < mPointCount; i++){
// 			Vec3d curP = mCurPositions[i];
// 			Vec3d d = curP - Vec3d(0.5, 11, 0.6);
// 			double dis = d.length();
// 			if(dis < 1.0){
// 				mCurPositions[i] += d.normalize_cond() * (1 - dis);
// 				mCurVelocities[i] = Vec3d(0,0,0);
// 			}
// 		}		
// 		for(size_t i = 0; i < mPointCount; i++){
// 			Vec3d& p = mCurPositions[i];
// 			Vec3d& v = mCurVelocities[i];
// 			double* d = (p - v).values_;
// 			float origin[3] = {(float)(p.values_[0]), (float)(p.values_[1]), (float)(p.values_[2])};
// 			float dir[3] = {(float)(d[0]), (float)(d[1]), (float)(d[2])};
// 			float cop[3];
// 			CollisionModel3D_ collision = mCollisionPartOnHuman[i];
// 			if(collision->rayCollision(origin, dir)){
// 				collision->getCollisionPoint(cop, false);
// 				Vec3d collisionPoint(cop[0], cop[1], cop[2]);
// 				if((collisionPoint - p).length() < 0.1){
// 					p = collisionPoint;
// 					v = -v;
// 				}
// 			}
// 		}

		for(size_t i = 0; i < mPointCount; i++){
			Vec3d& p = mCurPositions[i];
			Vec3d& v = mCurVelocities[i];
			Vec3d temp = v;
			Vec3d q = p + (-temp).normalize_cond() * 0.05;
			LineSegment seg(Point(p.values_[0],p.values_[1],p.values_[2]),
				Point(q.values_[0],q.values_[1],q.values_[2]));
			Vec3d intersectionPoint;
			size_t intersectTriangleIndex;
			if(mAABBTree->fastIntersectionTest(seg)){
				mAABBTree->intersection(seg, intersectionPoint,intersectTriangleIndex);				
				
				Vec3d faceVers[3];
				int _v = 0;
				for(Mesh::FaceVertexIter fv = mHuman->fv_begin(Mesh::FaceHandle(intersectTriangleIndex));
					fv.is_valid(); fv++){
						faceVers[_v++] = mHuman->point(*fv);
				}
				Vec3d faceNormal = (faceVers[0] - faceVers[1]) % (faceVers[1] - faceVers[2]);

// 				if(((intersectionPoint - p) | faceNormal) < 0 )
// 					continue;

				Vec3d newV = -v;
				Vec3d axis = newV % faceNormal;
				double angle = -2* acos( (newV|faceNormal) / (newV.length()*faceNormal.length()));				
				Quaternion q(axis, angle);
				q.rotate(newV);
				p = intersectionPoint;
				v = newV;
			}
		}
		/* 存储m_dt时间步长后的状态 */
// 		mCurPositions = nextPositions;
// 		mCurVelocities = nextVelocities;

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

// void GarmentPhysicalSimulator::computeCollisionParts()
// {
// 	if(mGarment == nullptr){
// 		return;
// 	}
// 	std::cout << "computeCollisionParts: ";
// 	std::vector<Vec3d> pointCloud;
// 	pointCloud.reserve(mHuman->n_vertices());
// 	for(auto it = mHuman->vertices_begin(); it != mHuman->vertices_end(); it++){
// 		pointCloud.push_back(mHuman->point(*it));
// 	}
// 	KNNSHelper kNNS(pointCloud);
// 	std::vector<KNNSHelper::Result> results;
// 	for(auto it = mGarment->vertices_begin(); it != mGarment->vertices_end(); it++){		
// 		Vec3d& query = mGarment->point(*it);
// 		CollisionModel3D_ collision = std::make_shared<COLDET::CollisionModel3DImpl>(true);
// 		mCollisionPartOnHuman.push_back(collision);
// 		std::vector<Tri> triangles;
// 		kNNS.kNeighborSearch(query, 5, results);
// 		std::unordered_set<size_t> hasFaceAdd;
// 		for(size_t i = 0; i < results.size(); i++){
// 			size_t nearestVer = results[i].index;
// 			for(auto vf = mHuman->vf_begin(Mesh::VertexHandle(nearestVer)); vf.is_valid(); vf++){
// 				if(hasFaceAdd.find(vf->idx()) != hasFaceAdd.end())
// 					continue;
// 				hasFaceAdd.insert(vf->idx());
// 				Tri tri;
// 				int cur = 0;
// 				for(auto fv = mHuman->fv_begin(*vf); fv.is_valid(); fv++){
// 					const Vec3d& p = mHuman->point(*fv);
// 					tri.v[cur][0] = (float)(p.values_[0]);
// 					tri.v[cur][1] = (float)(p.values_[1]);
// 					tri.v[cur][2] = (float)(p.values_[2]);
// 					cur += 1;
// 				}
// 				triangles.push_back(tri);				
// 			}
// 		}
// 		collision->setTriangleNumber(triangles.size());
// 		for(size_t i =0 ;i < triangles.size(); i++){
// 			Tri& tri = triangles[i];
// 			collision->addTriangle(tri.v[0], tri.v[1], tri.v[2]);
// 		}		
// 		collision->finalize();
// 		std::cout << it->idx() << " ";
// 	}
// }