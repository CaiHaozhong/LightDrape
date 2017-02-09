#include "GarmentFitter.h"
#include "Garment.h"
#include "Human.h"
#include "Segment.h"
#include "MeshDeformer.h"
#include "RegionFitter.h"
#include "VertexAlter.h"

GarmentFitter::GarmentFitter(void)
{
	this->mGarment = nullptr;
	this->mHuman = nullptr;
	this->mMeshDeformer = nullptr;
}

GarmentFitter::GarmentFitter( Garment_ garment )
{
	this->mGarment = garment;
	this->mHuman = nullptr;
	this->mMeshDeformer = nullptr;
}


GarmentFitter::~GarmentFitter(void)
{
}

void GarmentFitter::fit( Human_ human )
{
	if(human == nullptr || mGarment == nullptr){
		return;
	}
	mHuman = human;

	/* 先将衣服平移到人体身上 */
	translateGarment();

	Segment_ humSeg = human->getSegment();
	if(humSeg == nullptr){
		PRINTLN("Segment human first.");
		return;
	}
	Segment_ garSeg = mGarment->getSegment();
	if(garSeg == nullptr){
		PRINTLN("Segment garment first.");
		return;
	}
	std::vector<std::pair<int, Region_> > humanRegions = humSeg->getRegionsRaw();
	size_t count = humanRegions.size();
	for(size_t i = 0; i < count; i++){
		int bodyPart = humanRegions[i].first;
		Region_ bodyRegion = humanRegions[i].second;
		RegionFitter_ fitter = garSeg->getRegionFitter(bodyPart);
		if(fitter != nullptr){
			VertexAlter_ vertexAlter = fitter->fit(bodyRegion);
			if(mMeshDeformer == nullptr){
				PRINTLN("No avalible meshdeformer.");
				return;
			}
			mMeshDeformer->deformMesh(mGarment, vertexAlter);
		}
	}
}

void GarmentFitter::translateGarment()
{
	if(mGarment == nullptr || mHuman == nullptr){
		return;
	}
	Vec3d garAlign = mGarment->getAlignPoint();
	Vec3d humAlign = mHuman->getAlignPoint();
	Vec3d transVec = humAlign - garAlign;

	/* Translate Mesh */
	for(Mesh::VertexIter v_it = mGarment->vertices_begin();
		v_it != mGarment->vertices_end(); v_it++){
			Vec3d& p = mGarment->point(*v_it);
			p += transVec;
	}

	/* Translate Skeleton */
	Skeleton_ skeleton = mGarment->getSkeleton();
	Skeleton::NodeList nodes = skeleton->getNodeList();
	for(size_t i = 0; i < nodes.size(); i++){
		nodes[i]->point += transVec;
	}
}

void GarmentFitter::setGarment( Garment_ garment )
{
	this->mGarment = garment;
}

// void GarmentFitter::alterGarment()
// {
// 	size_t count = mGarSkeTrans.size();
// 	Skeleton_ skeleton = mGarment->getSkeleton();
// 	bool* hasAlter = new bool[mGarment->n_vertices()];
// 	memset(hasAlter, 0, mGarment->n_vertices());
// 	for(size_t i = 0; i < count; i++){
// 		std::pair<size_t, Vec3d>& alter = mGarSkeTrans[i];
// 		std::vector<size_t>& corrVertices 
// 			= skeleton->nodeAt(alter.first)->correspondanceIndices;
// 		size_t corCount = corrVertices.size();
// 		for(size_t c = 0; c < corCount; c++){
// 			size_t v = corrVertices[c];
// 			if(hasAlter[v]){
// 				PRINTLN("single vertex corrspondent to multi skenode");
// 				continue;
// 			}
// 			mGarment->alterVertex(v, alter.second);
// 			hasAlter[v] = true;
// 		}
// 	}
// 	delete [] hasAlter;
// }

void GarmentFitter::setMeshDeformer( MeshDeformer_ meshDeformer )
{
	this->mMeshDeformer = meshDeformer;
}
