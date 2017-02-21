#include "FullSkeletonFitter.h"
#include "Mesher.h"
#include "WatertightMesh.h"
#include "MeshSkeletonization.h"
#include "Skeleton.h"

FullSkeletonFitter::FullSkeletonFitter(void)
{
}

FullSkeletonFitter::FullSkeletonFitter( Region_ garmentRegion ):SimpleSkeletonFitter(garmentRegion){}


FullSkeletonFitter::~FullSkeletonFitter(void)
{
}

VertexAlter_ FullSkeletonFitter::fit( Region_ humanRegion )
{

}

std::string FullSkeletonFitter::getName()
{
	return "FullSkeletonFitter";
}

Skeleton_ RegionSkeletonizer::skeletonize( Region_ region )
{
	Mesher_ regionMesher = std::make_shared<Mesher>(region);
	Mesh_ mesh = regionMesher->getMesh();
	WatertightMesh_ watertightRegionMesh = std::make_shared<WatertightMesh>(mesh);
	MeshSkeletonization_ skeletonizer = smartNew(MeshSkeletonization);
	skeletonizer->skeletonize(watertightRegionMesh);
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
	return skeleton;
}
