#include "Region.h"
#include <fstream>
#include <string>
#include <map>
#include <tuple>
#include <queue>

Region::Region(void)
{
	mHasStartSetted = false;
}

Region::Region( Skeleton_ ske )
{
	setSkeleton(ske);
	mHasStartSetted = false;
}


Region::~Region(void)
{
}

void Region::setSkeleton( Skeleton_ ske )
{
	mSkeleton = ske;
}

std::set<size_t>::iterator Region::addVertex( size_t vertex )
{
	return mVertics.insert(vertex).first;
}

std::set<size_t>::iterator Region::addSkeleton( size_t skenode )
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
	if(mSkeleton == nullptr)
		return;
	

	size_t nodeCount = mSkeleton->nodeCount();
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
		Skeleton::IndexList& neighbors = mSkeleton->getNeighbors(cur);
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
	sortSkeleton();
	expandVertices();
	/* 确定起始骨骼节点 */
	confirmStartSkeNode();
	delete [] pre;
	delete [] hasVisited;
	delete [] inRegion;
}

void Region::sortSkeleton()
{		
	// Use Set, no need to sort
	/* TODO */
}

void Region::expandVertices()
{
	size_t vCount = mVertics.size();
	size_t skeCount = mSkeNodes.size();			
	for(std::set<size_t>::iterator it = mSkeNodes.begin();
		it != mSkeNodes.end(); it++){
			std::vector<size_t>& corrs = mSkeleton->nodeAt(*it)->correspondanceIndices;
			size_t corCount = corrs.size();
			for(size_t j = 0; j < corCount; j++){
				mVertics.insert(corrs[j]);
			}
	}
}

void Region::toSortedSkeleton( std::vector<size_t>& skeNodes,
							MultiNextNodeHandler_ multiNextNodeHandler)
{
	size_t cur = mStartSkeIndex;
	bool curValid = true;
	size_t last = cur;
	while(curValid){
		skeNodes.push_back(cur);
		Skeleton::IndexList& neighbors = mSkeleton->getNeighbors(cur);
		size_t neighborCount = neighbors.size();		
		std::vector<size_t> nextNodes;
		for(size_t i = 0; i < neighborCount; i++){
			size_t nei = neighbors[i];
			if(mSkeNodes.find(nei) != mSkeNodes.end()
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
// 
// 	skeNodes.push_back(mStartSkeIndex);
// 	size_t last = mStartSkeIndex;
// 	Skeleton::IndexList& neighbors = mSkeleton->getNeighbors(mStartSkeIndex);
// 	size_t nextCount = 0;
// 	size_t next = 0;
// 	for(size_t i = 0; i < neighbors.size(); i++){
// 		size_t nei = neighbors[i];
// 		if(mSkeNodes.find(nei) != mSkeNodes.end()){
// 			nextCount += 1;
// 			next = nei;
// 		}
// 	}
// 	bool hasNext = true;
// 	if(nextCount == 0){
// 		PRINTLN("no next node.");
// 		return;
// 	}
// 	else if(nextCount != 1){
// 		PRINTLN("multi next node");		
// 	}
// 	while(hasNext){
// 		skeNodes.push_back(next);
// 		size_t cur = next;
// 		Skeleton::IndexList& neighbors = mSkeleton->getNeighbors(next);
// 		size_t nextCount = 0;
// 		for(size_t i = 0; i < neighbors.size(); i++){
// 			size_t nei = neighbors[i];
// 			if(nei != last
// 				&& mSkeNodes.find(nei) != mSkeNodes.end()){
// 				nextCount += 1;
// 				next = nei;
// 			}
// 		}
// 		last = cur;
// 		if(nextCount == 0){
// 			hasNext = false;
// 		}
// 		if(nextCount != 0){
// 			PRINTLN("multi next node");
// 		}
// 	}
}

void Region::dump( std::string name )
{
#ifdef _DEBUG_
	std::string path = "../data/region/";
	std::ofstream out = std::ofstream(path + name + ".cg");
	size_t nodeCount = mSkeNodes.size();
	out << "# D:3 NV:" << nodeCount
		<< " NE:" << 0 << "\n";
	for(auto it = mSkeNodes.begin(); it != mSkeNodes.end(); it++){
		SkeletonNode_ skn = mSkeleton->nodeAt(*it);
		out << "v " << skn->point.values_[0] << " " << skn->point.values_[1] << " "
			<< skn->point.values_[2] << "\n";
	}
	out.close();
	out = std::ofstream(path + name + "_front.cg");
	out << "# D:3 NV:" << 1
		<< " NE:" << 0 << "\n";
	SkeletonNode_ skn = mSkeleton->nodeAt(mStartSkeIndex);
	out << "v " << skn->point.values_[0] << " " << skn->point.values_[1] << " "
		<< skn->point.values_[2] << "\n";
	out.close();
#endif
}

void Region::confirmStartSkeNode()
{
	Skeleton::IndexList neighbors = mSkeleton->getNeighbors(mStartSkeIndex);
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
		Skeleton::IndexList neighbors = mSkeleton->getNeighbors(cur);
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
