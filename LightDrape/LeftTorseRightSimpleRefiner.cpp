#include "LeftTorseRightSimpleRefiner.h"
#include "Region.h"
#include "Mesh.h"
#include "WatertightMesh.h"
#include <limits>

LeftTorseRightSimpleRefiner::LeftTorseRightSimpleRefiner(void)
{
}

LeftTorseRightSimpleRefiner::LeftTorseRightSimpleRefiner( MeshSegmenter_ meshSegmenter, Region_ left, Region_ torse, Region_ right )
{
	mMeshSegmenter = meshSegmenter;
	mLeft = left;
	mTorse = torse;
	mRight = right;
}


LeftTorseRightSimpleRefiner::~LeftTorseRightSimpleRefiner(void)
{
}

void LeftTorseRightSimpleRefiner::refine()
{
	if(mLeft == nullptr || mRight == nullptr || mTorse == nullptr)
		return;
	if(mLeft->getCircles().size() <= 0 || mRight->getCircles().size() <= 0){
		PRINTLN("In void LeftTorseRightSimpleRefiner::refine(), Region empty!");
		return;
	}
	refineRegion(mLeft);
	refineRegion(mRight);
	regionSub(mTorse, mLeft);
	regionSub(mTorse, mRight);
}

void LeftTorseRightSimpleRefiner::computeBoundedXZ( Region_ region,
												   double& minX, double& maxX,
												   double& minZ, double& maxZ, double& minY)
{
	double big = 100000, small = -100000;
	minX = big;
	maxX = small; // maxX用numeric_limits::min()的话，下面的if(p[0] > maxX)就一直过不去，这是为什么？而maxZ则不会，为什么？
	minZ = big;
	maxZ = small;
	minY = big;
	std::vector<size_t> vers;
	for(size_t i = 0; i < 13; i ++){
		std::vector<size_t>& tmp = region->getRegionSkeleton()->getNode(i)->vers;
		for(auto it = tmp.begin(); it != tmp.end(); it++){
			vers.push_back(*it);
		}
	}
	
	Mesh_ mesh = region->getMesh();

// 	std::string path = "../data/testSeg/ClothSeg/";
// 	char back[8];
// 	sprintf(back, "ps.cg");
// 	std::ofstream out = std::ofstream(path + mesh->getName() + back);
// 	out << "# D:3 NV:" << vers.size() << " NE:" << 0 << "\n";
// 	for(auto it = vers.begin(); it != vers.end(); it++){
// 			auto& p = mesh->point(Mesh::VertexHandle(*it)).values_;
// 			out << "v " << p[0] << " " << p[1] << " " << p[2] << "\n";
// 	}
// 	out.close();
	for(auto it = vers.begin(); it != vers.end(); it++){
		double* p = mesh->point(Mesh::VertexHandle(*it)).values_;
		if(p[0] < minX)
			minX = p[0];
		if(p[0] > maxX)
			maxX = p[0];
		if(p[2] < minZ)
			minZ = p[2];
		if(p[2] > maxZ)
			maxZ = p[2];
		if(p[1] < minY)
			minY = p[1];
	}
}

void LeftTorseRightSimpleRefiner::refineRegion( Region_ region )
{
	double minX, maxX, minZ, maxZ, minY;
	computeBoundedXZ(region, minX, maxX, minZ, maxZ, minY);
	Mesh_ mesh = region->getMesh();
	auto& torseVers = mTorse->getVertices();
	for(auto it = torseVers.begin(); it != torseVers.end(); it++){
		auto& p = mesh->point(Mesh::VertexHandle(*it)).values_;
		if((minX < p[0]) && (p[0] < maxX) /*&& (minZ < p[2]) && (p[2] < maxZ)*/ && (minY < p[1])){
			region->addVertex(*it);
		}
	}
}
