#include "WatertightMesh.h"
#include <fstream>


WatertightMesh::~WatertightMesh(void)
{
}

WatertightMesh::WatertightMesh( Mesh_ mesh ) :Mesh(*mesh)
{
	mOriginalMesh = mesh;
	HoleInfo holeInfo(this);
	holeInfo.getHoles();
	PRINT("holes Count: ");
	PRINTLN(holeInfo.holes()->size());
	holeInfo.fillAllHoles();	
	initProperty();
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

	std::ofstream out = std::ofstream("D:\\Develop\\project\\LightDrape\\C++\\data\\candidate.cg");	
	out << "# D:3 NV:" << candidatePoints.size() << " NE:" << 0 << "\n";
	for(size_t i = 0; i < candidatePoints.size(); i++){
		Vec3d& p = candidatePoints[i].first;
		out << "v " << p.values_[0] << " " << p.values_[1] << " " << p.values_[2] << "\n";
	}
	out.close();

	out = std::ofstream("D:\\Develop\\project\\LightDrape\\C++\\data\\center.cg");	
	out << "# D:3 NV:" << 1 << " NE:" << 0 << "\n";
	out << "v " << centerTopPoint.values_[0] << " " << centerTopPoint.values_[1] << " " << centerTopPoint.values_[2] << "\n";
	out.close();

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

void WatertightMesh::dumpSkeLinkMesh()
{
#ifdef _DEBUG_
	std::string path = "../data/skeLinkMesh/";
	for(size_t i = 0; i < mSkeleton->nodeCount(); i++){
		char back[8];
		sprintf(back, "%d.cg", i);
		std::ofstream out = std::ofstream(path + getName() + back);
		int edgeCount = mSkeleton->nodeAt(i)->correspondanceIndices.size();
		out << "# D:3 NV:" << n_vertices()+1
			<< " NE:" << edgeCount << "\n";
		for(Mesh::VertexIter vi = vertices_begin(); vi != vertices_end();
			vi++){
				Vec3d p = point(*vi);
				out << "v " << p.values_[0] << " " << p.values_[1] << " " << p.values_[2] << "\n";
		}
		SkeletonNode_ skn = mSkeleton->nodeAt(i);
		out << "v " << skn->point.values_[0] << " " << skn->point.values_[1] << " "
			<< skn->point.values_[2] << "\n";
		std::vector<size_t> cors = skn->correspondanceIndices;
		for(size_t k = 0; k < cors.size(); k++){
			out << "e " << cors.at(k)+1 << " " << n_vertices()+1 << "\n";
		}
		out.close();
	}
#endif
}

size_t WatertightMesh::getCorrSkeletonNode( size_t vertex )
{
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
