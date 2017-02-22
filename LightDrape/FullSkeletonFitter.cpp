#include <OpenMesh/Core/IO/MeshIO.hh>
#include "FullSkeletonFitter.h"
#include "Mesher.h"
#include "WatertightMesh.h"
#include "MeshSkeletonization.h"
#include "Skeleton.h"
#include "Config.h"
#include "VertexAlter.h"
#include <fstream>
#include "KNNSHelper.h"
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
	mGarmentSkeleton = regionSkeletonizer->skeletonize(mGarmentRegion, std::string("sleeve")+name);
	mHumanSkeleton = regionSkeletonizer->skeletonize(humanRegion, std::string("hand")+name);
	addSkeletonNodesToSet(mGarmentSkeleton, mGarSkeNodes);
	addSkeletonNodesToSet(mHumanSkeleton, mHumanSkeNodes);
	dumpRegionSkeletonLink(mGarmentSkeleton, mGarmentRegion, "leftSleeve");
	dumpRegionSkeletonLink(mHumanSkeleton, mHumanRegion, "leftHand");
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

Skeleton_ RegionSkeletonizer::skeletonize( Region_ region, std::string name )
{
// 	if(hasSkeletonCached(name)){
// 		return readSkeleton(name);
// 	}
	Mesher_ regionMesher = std::make_shared<Mesher>(region);
	Mesh_ mesh = regionMesher->getMesh();
	mesh->request_vertex_normals();
	mesh->request_face_normals();	
	mesh->requestAABB();
	Config_ config = Config::getInstance();
	OpenMesh::IO::write_mesh(*mesh, config->clothSegOutPath + name + "_region.obj");
	WatertightMesh_ watertightRegionMesh = std::make_shared<WatertightMesh>(mesh);
	OpenMesh::IO::write_mesh(*watertightRegionMesh, config->clothSegOutPath + name + "_watertightRegion.obj");
	MeshSkeletonization_ skeletonizer = smartNew(MeshSkeletonization);
	skeletonizer->set_quality_speed_tradeoff(0.5);
	skeletonizer->skeletonize(watertightRegionMesh);
	watertightRegionMesh->dumpSkeleton(name+"_region");
	Skeleton_ skeleton = watertightRegionMesh->getSkeleton();
	size_t nodeCount = skeleton->nodeCount();
	for(size_t i = 0; i < nodeCount; i++)	{
		SkeletonNode_ node = skeleton->nodeAt(i);
		std::vector<size_t>& corrs = node->correspondanceIndices;
		std::vector<size_t> regionCorrs;
		for(auto it = corrs.begin(); it != corrs.end(); it++){
			size_t waterIndex = *it;
			int originMeshIndex = watertightRegionMesh->getOriginVertex(waterIndex);
			if(originMeshIndex != -1){
				size_t regionIndex = regionMesher->getRegionVertex((size_t)originMeshIndex);
				regionCorrs.push_back(regionIndex);
			}
		}
		corrs.clear();
		corrs = regionCorrs;
	}
	cachSkeleton(skeleton, name);
	return skeleton;
}

std::string RegionSkeletonizer::getSkeletonFileName( std::string name )
{
	std::string path = "..\\data\\skeleton_cache\\";
	return path + name + ".ske";
}

bool RegionSkeletonizer::hasSkeletonCached( std::string name )
{
	name = getSkeletonFileName(name);
	FILE* file = fopen(name.c_str(), "r");
	if(file != nullptr){
		fclose(file);
		return true;
	}
	else{
		return false;
	}
}

Skeleton_ RegionSkeletonizer::readSkeleton( std::string file )
{
	Skeleton_ skeleton = smartNew(Skeleton);

	std::ifstream input(getSkeletonFileName(file));
	int numVertices, numEdges, correspondance, correspondanceCount;
	std::string ignore;
	input >> numVertices;
	while(numVertices--){
		SkeletonNode_ node = smartNew(SkeletonNode);
		input >> ignore
			>> node->point.values_[0] >> node->point.values_[1] >> node->point.values_[2]
		>> ignore
			>> node->delta.values_[0] >> node->delta.values_[1] >> node->delta.values_[2]
		>> ignore
			>> correspondanceCount;
		while (correspondanceCount--){
			input >> correspondance;
			node->correspondanceIndices.push_back(correspondance);
		}
		skeleton->addNode(node);
	}
	input >> numEdges;
	while(numEdges--){
		SkeletonEdge_ edge = smartNew(SkeletonEdge);
		input >> edge->sourceVertex >> edge->targetVertex;
		skeleton->addEdge(edge);
	}
	input.close();
	return skeleton;
}

bool RegionSkeletonizer::cachSkeleton( Skeleton_ skeleton, std::string file )
{
	size_t nodeCount = skeleton->nodeCount();
	size_t edgeCount = skeleton->edgeCount();
	typedef Vec3d Point;
	std::ofstream out(getSkeletonFileName(file));
	out << nodeCount << "\n";
	for(size_t i = 0; i < nodeCount; i++){
		SkeletonNode_ node = skeleton->nodeAt(i);
		double* point = node->point.values_;
		double* delta = node->delta.values_;
		Skeleton::IndexList& correspondanceList = node->correspondanceIndices;
		size_t correspondanceCount = correspondanceList.size();
		out << "point "
			<< point[0] << ' ' << point[1] << ' ' << point[2] 
		<< " delta "
			<< delta[0] << ' ' << delta[1] << ' ' << delta[2]
		<< " correspondance "	
			<< correspondanceCount << ' ';
		for (size_t j = 0; j < correspondanceCount; j++)
		{
			out << correspondanceList.at(j) << ' ';
		}
		out << "\n";	
	}

	out << edgeCount << "\n";

	for(size_t i = 0; i < edgeCount; i++){
		SkeletonEdge_ edge = skeleton->edgeAt(i);
		out << edge->sourceVertex << ' ' << edge->targetVertex << "\n";
	}

	out.close();
	return true;
}
