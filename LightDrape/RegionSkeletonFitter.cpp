#include "RegionSkeletonFitter.h"
#include "Region.h"
#include "Quaternion.h"
#include "VertexAlter.h"
#include "WatertightMesh.h"
#include <unordered_map>
RegionSkeletonFitter::RegionSkeletonFitter(void)
{
	mGarmentRegion = nullptr;
	mHumanRegion = nullptr;
}

RegionSkeletonFitter::RegionSkeletonFitter( Region_ garmentRegion ) : RegionFitter(garmentRegion)
{
	mHumanRegion = nullptr;
}


RegionSkeletonFitter::~RegionSkeletonFitter(void)
{
}

VertexAlter_ RegionSkeletonFitter::fit( Region_ humanRegion )
{
	mHumanRegion = humanRegion;
	std::vector< std::pair<size_t, Vec3d> > garSkeTrans;
	std::vector< std::pair<size_t, std::pair<Vec3d, double> > > garSkeRotate;
	fitSkeleton(garSkeTrans, garSkeRotate);
	VertexAlter_ ret = smartNew(VertexAlter);

// 	size_t skeCount = garSkeTrans.size();
// 	for(size_t i = 0; i < skeCount; i++){
// 		skeleton->getNode(garSkeTrans[i].first)->center += garSkeTrans[i].second;
// 	}
//	mGarmentRegion->dumpRegionSkeleton("garTransRegion");
	computeTranslation(ret, garSkeTrans);
	computeRotation(ret, garSkeRotate);
	return ret;
}

std::string RegionSkeletonFitter::getName()
{
	return "RegionSkeletonFitter";
}

void RegionSkeletonFitter::fitSkeleton( std::vector< std::pair<size_t, Vec3d> >& garSkeTrans, 
									   std::vector< std::pair<size_t, std::pair<Vec3d, double> > >& garSkeRotate )
{
	if( mGarmentRegion == nullptr || mHumanRegion == nullptr){
		PRINTLN("In SimpleSkeletonFitter, regions should not be null.");
		return;
	}
	RegionSkeleton_ humSkeleton = mHumanRegion->getRegionSkeleton();
	RegionSkeleton_ garSkeleton = mGarmentRegion->getRegionSkeleton();
	size_t humanSkeCount = humSkeleton->count();
	size_t garSkeCount = garSkeleton->count();

	size_t garStartSke = 0, humStartSke = 0;
	garSkeTrans.push_back(std::make_pair(garStartSke,
		humSkeleton->getNode(humStartSke)->center - garSkeleton->getNode(garStartSke)->center));
	double garLen = 0;
	double humLen = 0;
	size_t humCurIndex = humStartSke;
	double cachedLen = 0;
	for(size_t i = garStartSke+1; i < garSkeCount; i++){
		RegionSkeletonNode_ garCurNode = garSkeleton->getNode(i);
		RegionSkeletonNode_ garlastNode = garSkeleton->getNode(i-1);
//		garLen += garCurNode->height - garlastNode->height;
		garLen += (garCurNode->center-garlastNode->center).length();
		while(humLen < garLen){
			humCurIndex += 1;
			if(humCurIndex >= humanSkeCount)
				break;
//			cachedLen = humSkeleton->getNode(humCurIndex)->height - humSkeleton->getNode(humCurIndex-1)->height;
			cachedLen = (humSkeleton->getNode(humCurIndex)->center
				-humSkeleton->getNode(humCurIndex-1)->center).length();
			humLen += cachedLen;
		}
		if(humCurIndex >= humanSkeCount) // 假设袖子比手臂短
			break;
		Vec3d hp1 = humSkeleton->getNode(humCurIndex-1)->center;//humSkeleton->nodeAt(humOrderSkes[humCurIndex-1])->point;
		Vec3d hp2 = humSkeleton->getNode(humCurIndex)->center;//humSkeleton->nodeAt(humOrderSkes[humCurIndex])->point;
		Vec3d transVec = hp2 + (hp1-hp2) * (humLen-garLen)/cachedLen;
		garSkeTrans.push_back(std::make_pair(i, transVec - garCurNode->center));

		/* 计算骨骼旋转量 */
		Vec3d humanVec = hp2-hp1;
		const Vec3d& garCurPoint = garCurNode->center;//garSkeleton->nodeAt(curSke)->point;
		Vec3d garVec = garCurPoint - garSkeleton->getNode(i-1)->center;//garSkeleton->nodeAt(garOrderedSkes[i-1])->point;
		if(i + 1 < garSkeCount){
			garVec += (garSkeleton->getNode(i+1)->center - garCurPoint);
			garVec *= 0.5;
		}
		Vec3d axis = humanVec % garVec;
		double angle = -acos((humanVec|garVec)/(humanVec.length()*garVec.length()));
		garSkeRotate.push_back(std::make_pair(i, std::make_pair(axis, angle)));
	}
}


void RegionSkeletonFitter::computeTranslation( VertexAlter_ ret,
											  std::vector< std::pair<size_t, Vec3d> >& garSkeTrans )
{	
	size_t skeCount = garSkeTrans.size();
	WatertightMesh_ mesh = mGarmentRegion->getMesh();
	RegionSkeleton_ skeleton = mGarmentRegion->getRegionSkeleton();
	std::unordered_map<size_t, std::vector<size_t> > verMapSkes;
	verMapSkes.reserve(mGarmentRegion->getVertices().size());
	for(size_t i = 0; i < skeCount; i++){
		auto& vers = skeleton->getNode(garSkeTrans[i].first)->vers;
		for(auto it = vers.begin(); it != vers.end(); it++){
			verMapSkes[*it].push_back(i);
		}
	}
	for(auto it = verMapSkes.begin(); it != verMapSkes.end(); it++){
		size_t ver = it->first;
		std::vector<size_t>& skes = it->second;
		std::vector<double> ske_ver_dis;
		double sumDis = 0;
		ske_ver_dis.reserve(skes.size());
		Vec3d delta(0, 0, 0);
		for(auto ske_it = skes.begin(); ske_it != skes.end(); ske_it++){
/*			delta += garSkeTrans[*ske_it].second;*/
			double dis = (mesh->point(Mesh::VertexHandle(ver)) - skeleton->getNode(garSkeTrans[*ske_it].first)->center).length();
			ske_ver_dis.push_back(dis);
			sumDis += dis;
		}
		for(size_t i = 0; i < skes.size(); i++){
			delta += garSkeTrans[skes[i]].second * ske_ver_dis[i] / sumDis;
		}
		ret->add(ver, delta);
	}
	
// 	size_t skeCount = garSkeTrans.size();
// 	std::vector< size_t > involvedSkeNode;
// 	involvedSkeNode.reserve(3);
// 	for(size_t i = 0; i < skeCount; i++){
// 		involvedSkeNode.clear();
// 		involvedSkeNode.push_back(i);
// 		if((int)i - 1 >= 0 ){
// 			involvedSkeNode.push_back(i-1);
// 		}
// 		if(i + 1 < skeCount){
// 			involvedSkeNode.push_back(i+1);
// 		}
// 		RegionSkeletonNode_ skeNode = skeleton->getNode(garSkeTrans[i].first);
// 		std::vector<size_t>& corVertices = skeNode->vers;
// 		size_t corCount = corVertices.size();
// 		for(auto it = corVertices.begin(); it !=  corVertices.end(); it++){
// 			size_t v = *it;
// 			Vec3d& vPoint = mesh->point(Mesh::VertexHandle(v));
// 			double sumDis = 0;
// 			std::vector<double> involvedDis;
// 			involvedDis.reserve(involvedSkeNode.size());
// 			for(auto ske_it = involvedSkeNode.begin();
// 				ske_it != involvedSkeNode.end(); ske_it++){
// 					Vec3d& skeP = skeleton->getNode(garSkeTrans[*ske_it].first)->center;
// 					double dis = (vPoint-skeP).length();
// 					involvedDis.push_back(dis);
// 					sumDis += dis;
// 			}
// 			Vec3d vDelta = Vec3d(0,0,0);
// 			for(size_t j = 0; j < involvedDis.size(); j++){
// 				vDelta += involvedDis[j]/sumDis * garSkeTrans[involvedSkeNode[j]].second;
// 			}
// 			ret->add(v, vDelta);
// 		}
// 	}
}

void RegionSkeletonFitter::computeRotation( VertexAlter_ ret,
										   std::vector< std::pair<size_t, std::pair<Vec3d, double> > >& garSkeRotates )
{
	WatertightMesh_ mesh = mGarmentRegion->getMesh();
	RegionSkeleton_ skeleton = mGarmentRegion->getRegionSkeleton();
	size_t count = garSkeRotates.size();
	VertexAlter_ rotateResult = smartNew(VertexAlter);
	for(size_t i = 0; i < count; i++){
		size_t skeNode = garSkeRotates[i].first;
		const std::pair<Vec3d, double>& rotation = garSkeRotates[i].second;
		const std::vector<size_t>& corrs = skeleton->getNode(skeNode)->vers;
		rotateCircle(rotateResult, mesh, corrs, skeleton->getNode(skeNode)->center, rotation.first, rotation.second);
	}
	std::vector<std::pair<size_t, Vec3d> >& vertexDelta = rotateResult->vertexDelta;
	std::unordered_map<size_t, std::pair<Vec3d, size_t> > verDeltaMap;
	verDeltaMap.reserve(mGarmentRegion->getVertices().size());
	for(size_t i = 0; i < vertexDelta.size(); i++){
		size_t ver = vertexDelta[i].first;
		Vec3d& d = vertexDelta[i].second;
		if(verDeltaMap.find(ver) == verDeltaMap.end()){
			verDeltaMap.insert(std::make_pair(ver, std::make_pair(d, 1)));
		}
		else{
			auto& val = verDeltaMap[ver];
			val.first += d;
			val.second += 1;
		}
	}
	for(auto it = verDeltaMap.begin(); it != verDeltaMap.end(); it++){
		ret->add(it->first, it->second.first / it->second.second);
	}
}
