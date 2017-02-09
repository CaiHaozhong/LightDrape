#include "Region.h"
#include <fstream>
#include <string>
#include <map>
#include <tuple>
#include <queue>
#include "WatertightMesh.h"
#include "LevelSet.h"

Region::Region(void)
{
	mHasStartSetted = false;
	mMesh = nullptr;
}

Region::Region( WatertightMesh_ mesh )
{	
	mHasStartSetted = false;
	setMesh(mesh);
}


Region::~Region(void)
{
}

void Region::setMesh( WatertightMesh_ mesh )
{
	mMesh = mesh;
}

std::set<size_t>::iterator Region::addVertex( size_t vertex )
{
	return mVertics.insert(vertex).first;
}

std::set<size_t>::iterator Region::addSkeletonNode( size_t skenode )
{
	return mSkeNodes.insert(skenode).first;
}

void Region::setPossibleStart( size_t start )
{
	mStartSkeIndex = start;
	mHasStartSetted = true;
}

size_t Region::getStart()
{
	return mStartSkeIndex;
}

std::set<size_t>& Region::getVertices()
{
	return mVertics;
}

std::set<size_t>& Region::getSkeNodes()
{
	return mSkeNodes;
}

void Region::expand()
{
	if(mMesh == nullptr)
		return;
	/* expand skeleton */
	Skeleton_ skeleton = mMesh->getSkeleton();
	size_t nodeCount = skeleton->nodeCount();
	bool* inRegion = new bool[nodeCount];
	memset(inRegion, false, nodeCount);
	size_t curSize = mSkeNodes.size();
	for(std::set<size_t>::iterator it = mSkeNodes.begin();
		it != mSkeNodes.end(); it++){
			inRegion[*it] = true;
	}
	bool* hasVisited = new bool[nodeCount];
	memset(hasVisited, false, nodeCount);
	size_t* pre = new size_t[nodeCount];		
	std::queue<size_t> Q;
	size_t source = mStartSkeIndex;
	hasVisited[source] = true;
	Q.push(source);
	while(!Q.empty()){
		size_t cur = Q.front();
		Q.pop();
		Skeleton::IndexList& neighbors = skeleton->getNeighbors(cur);
		size_t neiCount = neighbors.size();
		for(size_t i = 0; i < neiCount; i++){
			size_t nei = neighbors[i];
			if(hasVisited[nei]){
				continue;
			}
			if(!inRegion[nei]){
				pre[nei] = cur;
			}
			else{
				size_t preNode = cur;
				while(!inRegion[preNode]){
					mSkeNodes.insert(preNode);
					inRegion[preNode] = true;
					preNode = pre[preNode];
				}
			}
			hasVisited[nei] = true;
			Q.push(nei);
		}
	}
	/* expand vertex */
	expandVertices();

	/* 确定起始骨骼节点 */
	confirmStartSkeNode();
	delete [] pre;
	delete [] hasVisited;
	delete [] inRegion;
}

void Region::expandVertices()
{
	if(mMesh == nullptr) return;
	Skeleton_ skeleton = mMesh->getSkeleton();
	size_t vCount = mVertics.size();
	size_t skeCount = mSkeNodes.size();			
	for(std::set<size_t>::iterator it = mSkeNodes.begin();
		it != mSkeNodes.end(); it++){
			std::vector<size_t>& corrs = skeleton->nodeAt(*it)->correspondanceIndices;
			size_t corCount = corrs.size();
			for(size_t j = 0; j < corCount; j++){
				mVertics.insert(corrs[j]);
			}
	}
}

void Region::dump( std::string name )
{
#ifdef _DEBUG_
	if(mMesh == nullptr) return;
	Skeleton_ skeleton = mMesh->getSkeleton();
	std::string path = "../data/region/";
	std::ofstream out = std::ofstream(path + name + ".cg");
	size_t nodeCount = mSkeNodes.size();
	out << "# D:3 NV:" << nodeCount
		<< " NE:" << 0 << "\n";
	for(auto it = mSkeNodes.begin(); it != mSkeNodes.end(); it++){
		SkeletonNode_ skn = skeleton->nodeAt(*it);
		out << "v " << skn->point.values_[0] << " " << skn->point.values_[1] << " "
			<< skn->point.values_[2] << "\n";
	}
	out.close();
	out = std::ofstream(path + name + "_front.cg");
	out << "# D:3 NV:" << 1
		<< " NE:" << 0 << "\n";
	SkeletonNode_ skn = skeleton->nodeAt(mStartSkeIndex);
	out << "v " << skn->point.values_[0] << " " << skn->point.values_[1] << " "
		<< skn->point.values_[2] << "\n";
	out.close();
#endif
}

void Region::confirmStartSkeNode()
{
	if(mMesh == nullptr) return;
	Skeleton_ skeleton = mMesh->getSkeleton();
	Skeleton::IndexList neighbors = skeleton->getNeighbors(mStartSkeIndex);
	size_t neiCount = neighbors.size();
	size_t inRegionCount = 0;
	for(size_t i = 0; i < neiCount; i++){
		size_t nei = neighbors[i];
		if(mSkeNodes.find(nei) != mSkeNodes.end()){
			inRegionCount += 1;
		}
	}
	/* 只有一个相邻的节点在该Region中，因此是起始节点 */
	if(inRegionCount == 1){
		return;
	}

	std::map<size_t, size_t > nodeStep;
	std::set<size_t> hasVisit;
	std::queue<size_t> Q;
	hasVisit.insert(mStartSkeIndex);
	nodeStep[mStartSkeIndex] = 0;
	Q.push(mStartSkeIndex);
	std::vector< std::pair<size_t, size_t> > endNodeSteps;
	while (!Q.empty()){
		size_t cur = Q.front();
		Q.pop();
		Skeleton::IndexList neighbors = skeleton->getNeighbors(cur);
		size_t neiCount = neighbors.size();
		bool hasUnVisitInRegion = false;
		for(size_t i = 0; i < neiCount; i++){
			size_t nei = neighbors[i];
			if(hasVisit.find(nei) != hasVisit.end()) // 已经访问过
				continue;			
			if(mSkeNodes.find(nei) != mSkeNodes.end()){ // In Region
				hasUnVisitInRegion = true;
				nodeStep[nei] = nodeStep[cur]+1;
				hasVisit.insert(nei);
				Q.push(nei);
			}			
		}
		if(!hasUnVisitInRegion){
			endNodeSteps.push_back(std::make_pair(cur, nodeStep[cur]));
		}
	}
	if(endNodeSteps.size() == 0){
		PRINTLN("confirm fail. endNodeSteps should not be empty.");
		return;
	}
	typedef std::pair<size_t, size_t> NS;
	std::sort(endNodeSteps.begin(), endNodeSteps.end(),[](const NS& a, const NS& b){
		return a.second < b.second;
	});
	mStartSkeIndex = endNodeSteps[0].first;
}

bool Region::hasStartSetted()
{
	return mHasStartSetted;
}

Skeleton_ Region::getSkeleton()
{
	if(mMesh == nullptr)
		return nullptr;
	return mMesh->getSkeleton();
}

WatertightMesh_ Region::getMesh()
{
	return mMesh;
}

size_t Region::addCircle( LevelCircle_ circle )
{
	mLevelCircles.push_back(circle);
	return mLevelCircles.size();
}

size_t Region::getCircleCount() const
{
	return mLevelCircles.size();
}

std::vector<LevelCircle_>& Region::getCircles()
{
	return mLevelCircles;
}

size_t Region::getVerticesSize() const
{
	return mVertics.size();
}
