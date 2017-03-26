#include "Skeleton.h"
#include <fstream>

SkeletonNode::SkeletonNode()
{
	delta = Vec3d(0,0,0);
}

SkeletonEdge::SkeletonEdge( size_t s, size_t t )
{
	sourceVertex = s;
	targetVertex = t;
}

Skeleton::Skeleton()
{

}

Skeleton::~Skeleton()
{
	clear();
}

size_t Skeleton::addNode( SkeletonNode_ node )
{
	mNodeList.push_back(node);
	mAdjacencyList.push_back(IndexList());
	return mNodeList.size()-1;
}

size_t Skeleton::addEdge( SkeletonEdge_ edge )
{
	mEdgeList.push_back(edge);
	mAdjacencyList[edge->sourceVertex].push_back(edge->targetVertex);
	mAdjacencyList[edge->targetVertex].push_back(edge->sourceVertex);
	return mEdgeList.size()-1;
}

SkeletonNode_ Skeleton::nodeAt( size_t i ) const
{
	if(i < mNodeList.size()){
		return mNodeList.at(i);
	}
	return nullptr;
}

SkeletonEdge_ Skeleton::edgeAt( size_t i ) const
{
	if(i < mEdgeList.size()){
		return mEdgeList.at(i);
	}
	return nullptr;
}

void Skeleton::clear()
{
	mNodeList.clear();
	mEdgeList.clear();
	mAdjacencyList.clear();
}

bool Skeleton::isNodeNeighbor( size_t node1, size_t node2 )
{
	if(node1 == node2){
		return true;
	}
	else{
		IndexList& neighbors = mAdjacencyList[node1];
		if(std::find(neighbors.begin(), neighbors.end(), node2)
			!= neighbors.end()){
				return true;
		}
	}
	return false;
}

size_t Skeleton::intervalNodeCount( size_t node1, size_t node2 )
{
	if(node1 == node2){
		return 0;
	}
	bool* hasVisited = new bool[this->nodeCount()];
	size_t* steps = new size_t[this->nodeCount()];
	memset(hasVisited, 0, this->nodeCount());
	memset(steps, 0, this->nodeCount());
	std::queue<size_t> Q;
	hasVisited[node1] = true;
	steps[node1] = 0;
	Q.push(node1);
	while(!Q.empty()){
		size_t cur = Q.front();
		Q.pop();
		IndexList& neighs = mAdjacencyList[cur];
		for(size_t i = 0; i < neighs.size(); i++){
			size_t n = neighs[i];
			if(hasVisited[n]){
				continue;
			}
			if(n == node2){					
				size_t ret = steps[cur]+1;
				delete [] hasVisited;
				delete [] steps;
				return ret;
			}
			else{
				steps[n] = steps[cur]+1;
			}
			hasVisited[n] = true;
			Q.push(n);
		}
	}
	delete [] hasVisited;
	delete [] steps;
	return -1;
}

double Skeleton::nodeEuclideanDis( size_t node1, size_t node2 )
{
	Vec3d p1 = this->nodeAt(node1)->point;
	Vec3d p2 = this->nodeAt(node2)->point;
	return (p1-p2).length();
}

void Skeleton::dump( std::string filename )
{
#ifdef _DEBUG_	
	std::ofstream out = std::ofstream(filename);
	size_t nodeCount = this->nodeCount();
	size_t edgeCount = this->edgeCount();
	out << "# D:3 NV:" << nodeCount
		<< " NE:" << edgeCount << "\n";
	for(size_t i = 0; i < nodeCount; i++){										
		SkeletonNode_ skn = this->nodeAt(i);
		out << "v " << skn->point.values_[0] << " " << skn->point.values_[1] << " "
			<< skn->point.values_[2] << "\n";		
	}
	for(size_t i = 0; i < edgeCount; i++){
		SkeletonEdge_ edge = this->edgeAt(i);
		out << "e " << edge->sourceVertex+1 << " " << edge->targetVertex+1 << "\n";
	}
	out.close();
#endif
}
