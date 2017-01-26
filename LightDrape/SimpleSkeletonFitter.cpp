#include "SimpleSkeletonFitter.h"
#include "VertexAlter.h"
#include "Region.h"
#include <algorithm>
#include "Skeleton.h"
#include "WatertightMesh.h"

size_t ChooseLongBranch::decide( size_t prev, std::vector<size_t>& nextNodes )
{
	std::vector< std::pair<size_t, size_t> > nodeLens;
	size_t nodeCount = nextNodes.size();
	for(size_t i = 0; i < nodeCount; i++){
		nodeLens.push_back(std::make_pair(nextNodes[i], length(prev, nextNodes[i])));
	}
	typedef std::pair<size_t, size_t> NL;
	std::sort(nodeLens.begin(), nodeLens.end(), [](const NL& a, const NL& b){
		return a.second > b.second;
	});
	return nodeLens[0].first;
}

size_t ChooseLongBranch::length( size_t prev, size_t node )
{
	Skeleton::IndexList& neighbors = mSkeleton->getNeighbors(node);
	size_t ret = 1;
	size_t neiCount = neighbors.size();
	/* 只有一个邻居或有两个以上的邻居都视为终点 */
	while(neiCount == 2){
		for(size_t i = 0; i < 2; i++){
			size_t nei = neighbors[i];
			if(nei != prev){
				prev = node;
				node = nei;
				ret += 1;
			}
		}
		neighbors = mSkeleton->getNeighbors(node);
		neiCount = neighbors.size();
	}
	return ret;
}

SimpleSkeletonFitter::SimpleSkeletonFitter(void)
{
}

SimpleSkeletonFitter::SimpleSkeletonFitter( Region_ garmentRegion ):RegionFitter(garmentRegion){}


SimpleSkeletonFitter::~SimpleSkeletonFitter(void)
{
}

VertexAlter_ SimpleSkeletonFitter::fit( Region_ humanRegion )
{
	std::vector< std::pair<size_t, Vec3d> > garSkeTrans;
	fitSkeleton(garSkeTrans, humanRegion);
	return computeVertexAlters(mGarmentRegion->getMesh(), garSkeTrans);
}

std::string SimpleSkeletonFitter::getName(){
	return "SimpleSkeletonFitter";
}

void SimpleSkeletonFitter::getSortedSkeleton( Region_ region, std::vector<size_t>& sortedSkeNodes,
							  MultiNextNodeHandler_ multiNextNodeHandler)
{
	Skeleton_ skeleton = region->getSkeleton();
	auto skeletonNodes = region->getSkeNodes();
	size_t cur = region->getStart();
	bool curValid = true;
	size_t last = cur;
	while(curValid){
		sortedSkeNodes.push_back(cur);
		Skeleton::IndexList& neighbors = skeleton->getNeighbors(cur);
		size_t neighborCount = neighbors.size();		
		std::vector<size_t> nextNodes;
		for(size_t i = 0; i < neighborCount; i++){
			size_t nei = neighbors[i];
			if(skeletonNodes.find(nei) != skeletonNodes.end()
				&& nei != last){					
					nextNodes.push_back(nei);
			}
		}
		if(nextNodes.size() == 0){
			curValid = false;
		}
		else{
			size_t next = nextNodes[0];
			if(nextNodes.size() != 1){							
				next = multiNextNodeHandler->decide(cur, nextNodes);
				char msg[100];				
				sprintf(msg, "multi next node on %d, decide %d as next node", cur, next);
				PRINTLN(msg);
			}
			last = cur;
			cur = next;
		}
	}
}

void SimpleSkeletonFitter::fitSkeleton( std::vector< std::pair<size_t, Vec3d> >& garSkeTrans, Region_ humanRegion )
{
	if( mGarmentRegion == nullptr || humanRegion == nullptr){
		PRINTLN("In SimpleSkeletonFitter, regions should not be null.");
		return;
	}
	Skeleton_ garSkeleton = mGarmentRegion->getSkeleton();
	Skeleton_ humSkeleton = humanRegion->getSkeleton();
	std::vector<size_t> garOrderedSkes;
	std::vector<size_t> humOrderSkes;
	getSortedSkeleton(mGarmentRegion, garOrderedSkes, std::make_shared<ChooseLongBranch>(garSkeleton));
	getSortedSkeleton(humanRegion, humOrderSkes, std::make_shared<ChooseLongBranch>(humSkeleton));
	size_t garStartSke = garOrderedSkes[0];
	size_t humStartSke = humOrderSkes[0];
	garSkeTrans.push_back(std::make_pair(garStartSke,
		humSkeleton->nodeAt(humStartSke)->point - garSkeleton->nodeAt(garStartSke)->point));
	double garLen = 0;
	double humLen = 0;
	size_t humCurIndex = 0;
	double cachedLen = 0;
	for(size_t i = 1; i < garOrderedSkes.size(); i++){
		size_t curSke = garOrderedSkes[i];
		garLen += garSkeleton->nodeEuclideanDis(garOrderedSkes[i-1], curSke);		
		while(humLen < garLen){
			humCurIndex += 1;
			if(humCurIndex >= humOrderSkes.size())
				break;
			cachedLen = humSkeleton->nodeEuclideanDis(humOrderSkes[humCurIndex-1],
				humOrderSkes[humCurIndex]);
			humLen += cachedLen;
		}
		if(humCurIndex >= humOrderSkes.size()) // 假设袖子比手臂短
			break;
		Vec3d hp1 = humSkeleton->nodeAt(humOrderSkes[humCurIndex-1])->point;
		Vec3d hp2 = humSkeleton->nodeAt(humOrderSkes[humCurIndex])->point;
		Vec3d transVec = hp2 + (hp1-hp2) * (humLen-garLen)/cachedLen;
		garSkeTrans.push_back(std::make_pair(curSke, transVec - garSkeleton->nodeAt(curSke)->point));
	}

}

VertexAlter_ SimpleSkeletonFitter::computeVertexAlters( WatertightMesh_ mesh, std::vector< std::pair<size_t, Vec3d> >& garSkeTrans )
{
	VertexAlter_ ret = smartNew(VertexAlter);
	Skeleton_ skeleton = mesh->getSkeleton();
	size_t skeCount = garSkeTrans.size();
	std::vector< size_t > involvedSkeNode;
	involvedSkeNode.reserve(3);
	for(size_t i = 0; i < skeCount; i++){
		involvedSkeNode.clear();
		involvedSkeNode.push_back(i);
		if((int)i - 1 >= 0 ){
			involvedSkeNode.push_back(i-1);
		}
		if(i + 1 < skeCount){
			involvedSkeNode.push_back(i+1);
		}
		SkeletonNode_ skeNode = skeleton->nodeAt(garSkeTrans[i].first);
		std::vector<size_t>& corVertices = skeNode->correspondanceIndices;
		size_t corCount = corVertices.size();
		for(auto it = corVertices.begin(); it !=  corVertices.end(); it++){
			size_t v = *it;
			Vec3d& vPoint = mesh->point(Mesh::VertexHandle(v));
			double sumDis = 0;
			std::vector<double> involvedDis;
			involvedDis.reserve(involvedSkeNode.size());
			for(auto ske_it = involvedSkeNode.begin();
				ske_it != involvedSkeNode.end(); ske_it++){
					Vec3d& skeP = skeleton->nodeAt(garSkeTrans[*ske_it].first)->point;
					double dis = (vPoint-skeP).length();
					involvedDis.push_back(dis);
					sumDis += dis;
			}
			Vec3d vDelta = Vec3d(0,0,0);
			for(size_t j = 0; j < involvedDis.size(); j++){
				vDelta += involvedDis[j]/sumDis * garSkeTrans[involvedSkeNode[j]].second;
			}
			ret->add(v, vDelta);
		}
	}
	return ret;
}
