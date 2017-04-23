#include <OpenMesh/Core/IO/MeshIO.hh>
#include "FullSkeletonFitter.h"
#include "Mesher.h"
#include "WatertightMesh.h"
#include "Skeleton.h"
#include "Config.h"
#include "VertexAlter.h"
#include <fstream>
#include "KNNSHelper.h"
#include "RegionSkeletonizer.h"
#include <ctime>
int FullSkeletonFitter::fitI = 0;
FullSkeletonFitter::FullSkeletonFitter(void)
{
}

FullSkeletonFitter::FullSkeletonFitter( Region_ garmentRegion ):SkeletonFitter(garmentRegion){}


FullSkeletonFitter::~FullSkeletonFitter(void)
{
}

VertexAlter_ FullSkeletonFitter::fit( Region_ humanRegion )
{
	mHumanRegion = humanRegion;
	RegionSkeletonizer_ regionSkeletonizer = smartNew(RegionSkeletonizer);
	char name[20];
	sprintf(name, "%d", fitI++);	
	mGarmentSkeleton = regionSkeletonizer->skeletonize(mGarmentRegion);
	mHumanSkeleton = regionSkeletonizer->skeletonize(humanRegion);
	Config_ config = Config::getInstance();	
	if(mGarmentRegion->getName() == "leftsleeve"){
		Mesh_ clothMesh = mGarmentRegion->getMesh();
		for(size_t i = 0; i < mGarmentSkeleton->nodeCount(); i++){
			char buf[100];
			_itoa(i, buf, 10);
			std::ofstream out = std::ofstream(config->clothSegOutPath + "corr/leftsleeve" + std::string(buf) + ".cg");
			SkeletonNode_ skn = mGarmentSkeleton->nodeAt(i);
			std::vector<size_t> cors = skn->correspondanceIndices;
			int corrCount = cors.size();
			out << "# D:3 NV:" << corrCount+1
				<< " NE:" << 0 << "\n";
			for(size_t k = 0; k < corrCount; k++){
				const Vec3d& p = clothMesh->point(Mesh::VertexHandle(cors[k]));
				out << "v " << p.values_[0] << " " << p.values_[1] << " " << p.values_[2] << "\n";
			}
			out << "v " << skn->point.values_[0] << " " << skn->point.values_[1] << " " << skn->point.values_[2] << "\n";
			out.close();
		}
	}
	if(humanRegion->getName() == "lefthand"){
		Mesh_ humanMesh = humanRegion->getMesh();
		for(size_t i = 0; i < mHumanSkeleton->nodeCount(); i++){
			char buf[100];
			_itoa(i, buf, 10);
			std::ofstream out = std::ofstream(config->clothSegOutPath + "corr/lefthand" + std::string(buf) + ".cg");
			SkeletonNode_ skn = mHumanSkeleton->nodeAt(i);
			std::vector<size_t> cors = skn->correspondanceIndices;
			int corrCount = cors.size();
			out << "# D:3 NV:" << corrCount+1
				<< " NE:" << 0 << "\n";
			for(size_t k = 0; k < corrCount; k++){
				const Vec3d& p = humanMesh->point(Mesh::VertexHandle(cors[k]));
				out << "v " << p.values_[0] << " " << p.values_[1] << " " << p.values_[2] << "\n";
			}
			out << "v " << skn->point.values_[0] << " " << skn->point.values_[1] << " " << skn->point.values_[2] << "\n";
			out.close();
		}
	}
	addSkeletonNodesToSet(mGarmentSkeleton, mGarSkeNodes);
	addSkeletonNodesToSet(mHumanSkeleton, mHumanSkeNodes);
// 	dumpRegionSkeletonLink(mGarmentSkeleton, mGarmentRegion, "leftSleeve");
// 	dumpRegionSkeletonLink(mHumanSkeleton, mHumanRegion, "leftHand");
	return SkeletonFitter::fit(humanRegion);


	//dumpRegionSkeletonLink(garSke, mGarmentRegion, "leftSleeve");

	//return smartNew(VertexAlter);
}

std::string FullSkeletonFitter::getName()
{
	return "FullSkeletonFitter";
}

void FullSkeletonFitter::dumpRegionSkeletonLink( Skeleton_ ske, Region_ region, std::string name )
{	
	std::string skeFile = "../data/testSeg/ClothSeg/skeletonLink/" + name + ".cg";
	std::ofstream skeOut = std::ofstream(skeFile);
	skeOut << "# D:3 NV:" << ske->nodeCount()
		<< " NE:" << 0 << "\n";
	Mesh_ mesh = region->getMesh();
	for(size_t i = 0; i < ske->nodeCount(); i++){
		std::string path = "../data/testSeg/ClothSeg/skeletonLink/" + name;
		char back[8];
		sprintf(back, "_%d.cg", i);
		std::ofstream out = std::ofstream(path + back);
		SkeletonNode_ skn = ske->nodeAt(i);
		std::vector<size_t> cors = skn->correspondanceIndices;
		int corrCount = cors.size();
		out << "# D:3 NV:" << corrCount+1
			<< " NE:" << corrCount << "\n";
		for(size_t k = 0; k < corrCount; k++){
			const Vec3d& p = mesh->point(Mesh::VertexHandle(cors[k]));
			out << "v " << p.values_[0] << " " << p.values_[1] << " " << p.values_[2] << "\n";
		}
		const Vec3d& skePoint = skn->point;
		out << "v " << skePoint.values_[0] << " " << skePoint.values_[1] << " "
			<< skePoint.values_[2] << "\n";
		skeOut << "v " << skePoint.values_[0] << " " << skePoint.values_[1] << " "
			<< skePoint.values_[2] << "\n";
		for(size_t k = 0; k < corrCount; k++){
			out << "e " << corrCount+1 << " " << k+1 << "\n";
		}
		out.close();
	}

	skeOut.close();

}

size_t FullSkeletonFitter::getStartSkeletonNode( Region_ region )
{
	/* 返回y值最高的骨骼节点 */
	Skeleton_ ske = getSkeleton(region);
	double maxY = -10000;
	size_t ret = 0;
	for(size_t i = 0; i < ske->nodeCount(); i++){
		double curY = ske->nodeAt(i)->point.values_[1];
		if(curY > maxY){
			maxY = curY;
			ret = i;
		}
	}
	return ret;
}

Skeleton_ FullSkeletonFitter::getSkeleton( Region_ region )
{
	if(region == mHumanRegion){
		return mHumanSkeleton;
	}
	else if(region == mGarmentRegion){
		return mGarmentSkeleton;
	}
	return nullptr;
}

std::set<size_t>& FullSkeletonFitter::getSkeletonNodesInRegion( Region_ region )
{
	if(region == mHumanRegion){
		return mHumanSkeNodes;
	}
	else if(region == mGarmentRegion){
		return mGarSkeNodes;
	}
	PRINTLN("In Correct return from getSkeletonNodesInRegion in FullSkeletonFitter");
	return mGarSkeNodes;
}

void FullSkeletonFitter::addSkeletonNodesToSet( Skeleton_ ske, std::set<size_t>& nodeSet )
{
	/* TO DO 
	 * 待重构
	 */
	for(size_t i = 0; i < ske->nodeCount(); i++){
		nodeSet.insert(i);
	}
}

void FullSkeletonFitter::getSortedSkeleton( Region_ region, std::vector<size_t>& sortedSkeNodes, MultiNextNodeHandler_ multiNextNodeHandler )
{
	Skeleton_ skeleton = getSkeleton(region);
	size_t nodeCount = skeleton->nodeCount();
	bool* hasVisited = new bool[nodeCount];
	size_t start = getStartSkeletonNode(region);
	sortedSkeNodes.push_back(start);		
	memset(hasVisited, 0, nodeCount);
	hasVisited[start] = true;
	Vec3d curNode = skeleton->nodeAt(start)->point;
	std::vector<Vec3d> pointCloud;
	pointCloud.reserve(nodeCount);
	for(size_t i = 0; i < nodeCount; i++){
		pointCloud.push_back(skeleton->nodeAt(i)->point);
	}
	KNNSHelper knns(pointCloud);	
	size_t searchCount = nodeCount / 2;
	for(size_t i = 0; i < nodeCount; i++){
		std::vector<KNNSHelper::Result> results;
		knns.kNeighborSearch(curNode, searchCount, results);
		std::sort(results.begin(), results.end(), [](const KNNSHelper::Result& a, const KNNSHelper::Result& b){
			return a.distance < b.distance;
		});
		size_t r = 0;
		size_t resultSize = results.size();
		while(r < resultSize && hasVisited[results[r].index]){
			r++;
		}
		if(r == resultSize)
			break;
		size_t newNode = results[r].index;
		sortedSkeNodes.push_back(newNode);
		hasVisited[newNode] = true;
		curNode = skeleton->nodeAt(newNode)->point;
	}
	delete [] hasVisited;
}