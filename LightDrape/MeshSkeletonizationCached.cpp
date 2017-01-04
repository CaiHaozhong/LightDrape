#include "MeshSkeletonizationCached.h"
#include "WatertightMesh.h"
#include <fstream>

MeshSkeletonizationCached::MeshSkeletonizationCached(void)
{
}


MeshSkeletonizationCached::~MeshSkeletonizationCached(void)
{
}

void MeshSkeletonizationCached::skeletonize( WatertightMesh_ mesh )
{
	if(hasSkeletonCached(mesh)){
		PRINTLN("Use cached skeleton.");
		Skeleton_ skeleton = readSkeleton(getSkeletonFileName(mesh));
		mesh->setSkeleton(skeleton);
		/** Set vertex property of skeleton node **/
		IntProperty_ vertexPropertySKN = smartNew(IntProperty);
		mesh->registerProperty(vertexPropertySKN);
		const Skeleton::NodeList& nodeList = skeleton->getNodeList();
		for(size_t i = 0; i < nodeList.size(); i++){
			std::vector<size_t>& corsVertices = nodeList[i]->correspondanceIndices;
			for(size_t j = 0; j < corsVertices.size(); j++){
				vertexPropertySKN->set(corsVertices[j], i);
			}
		}
		/* Set Property */
		mesh->setVertexPropertySKN(vertexPropertySKN);
	}
	else{
		MeshSkeletonization::skeletonize(mesh);			
		bool csuc = cachSkeleton(mesh->getSkeleton(), getSkeletonFileName(mesh));
		if(csuc)
			PRINTLN("Skeleton cached success!");
		else
			PRINTLN("Skeleton cached failed!");
	}
}

std::string MeshSkeletonizationCached::getSkeletonFileName( WatertightMesh_ mesh )
{
	std::string name = mesh->getName();
	std::string path = "..\\data\\skeleton_cache\\";
	return path + name + ".ske";
}

bool MeshSkeletonizationCached::hasSkeletonCached( WatertightMesh_ mesh )
{
	std::string name = getSkeletonFileName(mesh);
	FILE* file = fopen(name.c_str(), "r");
	if(file != nullptr){
		fclose(file);
		return true;
	}
	else{
		return false;
	}
}

Skeleton_ MeshSkeletonizationCached::readSkeleton( std::string file )
{
	Skeleton_ skeleton = smartNew(Skeleton);

	std::ifstream input(file);
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

bool MeshSkeletonizationCached::cachSkeleton( Skeleton_ skeleton, std::string file )
{
	size_t nodeCount = skeleton->nodeCount();
	size_t edgeCount = skeleton->edgeCount();
	typedef Vec3d Point;
	std::ofstream out(file);
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
