#include "RegionSkeletonizer.h"
#include "MeshSkeletonization.h"
#include "Region.h"
#include "Skeleton.h"
#include "Config.h"
#include "WatertightMesh.h"
#include "Mesher.h"


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
	//	OpenMesh::IO::write_mesh(*mesh, config->clothSegOutPath + name + "_region.obj");
	WatertightMesh_ watertightRegionMesh = std::make_shared<WatertightMesh>(mesh);
	//	OpenMesh::IO::write_mesh(*watertightRegionMesh, config->clothSegOutPath + name + "_watertightRegion.obj");
	MeshSkeletonization_ skeletonizer = smartNew(MeshSkeletonization);
	skeletonizer->set_quality_speed_tradeoff(0.5);
	skeletonizer->skeletonize(watertightRegionMesh);
	//	watertightRegionMesh->dumpSkeleton(name+"_region");
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
