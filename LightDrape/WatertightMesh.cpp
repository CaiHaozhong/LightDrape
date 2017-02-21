#include "WatertightMesh.h"
#include <fstream>
#include "KNNSHelper.h"

WatertightMesh::~WatertightMesh(void)
{
}

WatertightMesh::WatertightMesh( Mesh_ mesh ) :Mesh(*mesh)
{
	mOriginalMesh = mesh;
	mVertexPropertySKN = nullptr;
	mVertexPropertyGeoDis = nullptr;
	HoleInfo holeInfo(this);
	holeInfo.getHoles();
	PRINT("holes Count: ");
	PRINTLN(holeInfo.holes()->size());
	holeInfo.fillAllHoles();	
	initProperty();
	computeMap();
}

Skeleton_ WatertightMesh::getSkeleton() const
{
	return mSkeleton;
}

void WatertightMesh::setSkeleton( Skeleton_ val )
{
	mSkeleton = val;
}

Segment_ WatertightMesh::getSegment() const
{
	return mSegment;
}

void WatertightMesh::setSegment( Segment_ val )
{
	mSegment = val;
}

size_t WatertightMesh::getGeodesicSource()
{
	double maxYValue = getMax().values_[1];
	double minYValue = getMin().values_[1];
	double threshold = maxYValue - getAverageEdgeLength()*3;
	typedef std::pair<Vec3d, size_t> Node;
	std::vector<Node> candidatePoints;
	int cur = 0;
	double minX = std::numeric_limits<double>::max(),
		maxX = -minX, minZ = minX, maxZ = -minX;
	for (VertexIter v_it = vertices_begin();
		v_it != vertices_end(); ++v_it){
			Vec3d& p = this->point(*v_it);
			if(p.values_[1] >= threshold){
				candidatePoints.push_back(std::make_pair(p, cur));
				if(p.values_[0] > maxX) maxX = p.values_[0];
				if(p.values_[0] < minX) minX = p.values_[0];
				if(p.values_[2] > maxZ) maxZ = p.values_[2];
				if(p.values_[2] < minZ) minZ = p.values_[2];
			}
			cur++;
	}
	Vec3d centerTopPoint = Vec3d((maxX+minX)/2, maxYValue, (maxZ+minZ)/2);

// 	std::ofstream out = std::ofstream("D:\\Develop\\project\\LightDrape\\C++\\data\\candidate.cg");	
// 	out << "# D:3 NV:" << candidatePoints.size() << " NE:" << 0 << "\n";
// 	for(size_t i = 0; i < candidatePoints.size(); i++){
// 		Vec3d& p = candidatePoints[i].first;
// 		out << "v " << p.values_[0] << " " << p.values_[1] << " " << p.values_[2] << "\n";
// 	}
// 	out.close();
// 
// 	out = std::ofstream("D:\\Develop\\project\\LightDrape\\C++\\data\\center.cg");	
// 	out << "# D:3 NV:" << 1 << " NE:" << 0 << "\n";
// 	out << "v " << centerTopPoint.values_[0] << " " << centerTopPoint.values_[1] << " " << centerTopPoint.values_[2] << "\n";
// 	out.close();

	double dis = std::numeric_limits<double>::max();
	Node sourceNode;
	for(size_t i = 0; i < candidatePoints.size(); i++){
		Vec3d& curPoint = candidatePoints[i].first;
		double dx = (curPoint.values_[0]-centerTopPoint.values_[0]);
//		double dy = (curPoint.values_[1]-centerTopPoint.values_[1]);
		double dz = (curPoint.values_[2]-centerTopPoint.values_[2]);
		double d = dx*dx + dz*dz;
		if(d < dis){
			dis = d;
			sourceNode = candidatePoints[i];
		}
	}
	return sourceNode.second;

	// 		Mesh::VertexHandle vh = *vertices_begin();
	// 		double maxY = (double)_I32_MIN;
	// 		for(Mesh::VertexIter vIt = vertices_begin(); vIt != vertices_end(); vIt++){
	// 			double curY = this->point(*vIt).values_[1];
	// 			if( curY > maxY){
	// 				maxY = curY;
	// 				vh = *vIt;
	// 			}
	// 		}
	// 		return vh.idx();
}

void WatertightMesh::dumpSkeLinkMesh(size_t skeNode, std::string subPath)
{
#ifdef _DEBUG_
	std::string path = "../data/skeLinkMesh/" + subPath + "/";
	char back[8];
	sprintf(back, "%d.cg", skeNode);
	std::ofstream out = std::ofstream(path + getName() + back);
	SkeletonNode_ skn = mSkeleton->nodeAt(skeNode);
	std::vector<size_t> cors = skn->correspondanceIndices;
	int corrCount = cors.size();
	out << "# D:3 NV:" << corrCount+1
		<< " NE:" << corrCount << "\n";
	for(size_t k = 0; k < corrCount; k++){
		const Vec3d& p = this->point(Mesh::VertexHandle(cors[k]));
		out << "v " << p.values_[0] << " " << p.values_[1] << " " << p.values_[2] << "\n";
	}
	const Vec3d& skePoint = skn->point;
	out << "v " << skePoint.values_[0] << " " << skePoint.values_[1] << " "
		<< skePoint.values_[2] << "\n";

	for(size_t k = 0; k < corrCount; k++){
		out << "e " << corrCount+1 << " " << k+1 << "\n";
	}
	out.close();
#endif
}

size_t WatertightMesh::getCorrSkeletonNode( size_t vertex )
{
	if(mVertexPropertySKN == nullptr){
		PRINTLN("In WatertightMesh::getCorrSkeletonNode, should skeletonize first.");
		return 0;
	}
	return mVertexPropertySKN->get(vertex);
}

Vec3d WatertightMesh::getAlignPoint()
{
	return Vec3d(0,0,0);
}

void WatertightMesh::dumpSkeleton(std::string name)
{
#ifdef _DEBUG_
	std::string path = "../data/skeleton/";
	std::ofstream out = std::ofstream(path + name + ".cg");
	size_t nodeCount = mSkeleton->nodeCount();
	size_t edgeCount = mSkeleton->edgeCount();
	out << "# D:3 NV:" << nodeCount
		<< " NE:" << edgeCount << "\n";
	for(size_t i = 0; i < nodeCount; i++){										
		SkeletonNode_ skn = mSkeleton->nodeAt(i);
		out << "v " << skn->point.values_[0] << " " << skn->point.values_[1] << " "
			<< skn->point.values_[2] << "\n";		
	}
	for(size_t i = 0; i < edgeCount; i++){
		SkeletonEdge_ edge = mSkeleton->edgeAt(i);
		out << "e " << edge->sourceVertex+1 << " " << edge->targetVertex+1 << "\n";
	}
	out.close();
#endif
}

double WatertightMesh::getGeodesicDis( size_t ver )
{
	if(mVertexPropertyGeoDis == nullptr){
		PRINTLN("In WatertightMesh::getGeodesicDis, should compute geodesic first.");
		return -1;
	}
	return mVertexPropertyGeoDis->get(ver);
}

void WatertightMesh::computeMap()
{
	mOriginMapWatertight.resize(mOriginalMesh->n_vertices(), -1);
	mWatertightMapOrigin.resize(this->n_vertices(), -1);
	std::vector<Vec3d> pointCloud;
	pointCloud.reserve(this->n_vertices());
	for(auto it = this->vertices_begin(); it != this->vertices_end(); it++){
		pointCloud.push_back(this->point(*it));
	}
	KNNSHelper kNNS(pointCloud);
	KNNSHelper::Result result;
	for(auto it = mOriginalMesh->vertices_begin(); it != mOriginalMesh->vertices_end(); it++){
		Vec3d& query = mOriginalMesh->point(*it);
		kNNS.singleNeighborSearch(query, result);
		mOriginMapWatertight[it->idx()] = result.index;
		mWatertightMapOrigin[result.index] = it->idx();
	}
}

int WatertightMesh::getOriginVertex( size_t watertightVertex )
{
	if(watertightVertex >= this->n_vertices()){
		PRINTLN("watertightVertex out of bound");
		return -1;
	}
	return mWatertightMapOrigin[watertightVertex];
}

int WatertightMesh::getWatertightVertex( size_t originVertex )
{
	if(originVertex >= mOriginalMesh->n_vertices()){
		PRINTLN("originVertex out of bound");
		return -1;
	}
	return mOriginMapWatertight[originVertex];
}
