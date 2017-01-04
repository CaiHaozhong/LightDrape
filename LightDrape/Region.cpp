#include "Region.h"


Region::Region(void)
{
}

Region::Region( Skeleton_ ske )
{
	setSkeleton(ske);
}


Region::~Region(void)
{
}

void Region::setSkeleton( Skeleton_ ske )
{
	mSkeleton = ske;
}

void Region::addVertex( size_t vertex )
{
	mVertics.insert(vertex);
}

void Region::addSkeleton( size_t skenode )
{
	mSkeNodes.insert(skenode);
}

void Region::setStart( size_t start )
{
	mStartIndex = start;
}

size_t Region::getStart()
{
	return mStartIndex;
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
	size_t source = *(mSkeNodes.begin());
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
	delete [] pre;
	delete [] hasVisited;
	delete [] inRegion;
}

void Region::sortSkeleton()
{
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
