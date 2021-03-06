#include "GarmentFitter.h"
#include "Garment.h"
#include "Human.h"
#include "Segment.h"
#include "MeshDeformer.h"
#include "RegionFitter.h"
#include "VertexAlter.h"
#include "Config.h"
#include <ctime>

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
	//translateGarment();

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

		/* 衣服已经被Translate了，衣身没有必要适配人体了 */
		if(bodyPart == Segment::BODY_TORSE/* || bodyPart == Segment::BODY_LEFT_HAND*/)
			continue;

		Region_ bodyRegion = humanRegions[i].second;
		RegionFitter_ fitter = garSeg->getRegionFitter(bodyPart);
		if(fitter != nullptr){
			VertexAlter_ vertexAlter = fitter->fit(bodyRegion);
			if(mMeshDeformer == nullptr){
				PRINTLN("No avalible meshdeformer.");
				return;
			}
			clock_t _s = clock();
			mMeshDeformer->deformMesh(mGarment, vertexAlter);
			Config::getInstance()->deformTime += clock() - _s;
		}
	}

	/* 最后将衣服平移到人体身上 */
	//translateGarment();
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

	/* Translate Region Skeleton */
	std::vector< std::pair<int, Region_> >& regions = mGarment->getSegment()->getRegionsRaw();
	for(std::vector< std::pair<int, Region_> >::iterator it = regions.begin();
		it != regions.end(); it++){
			RegionSkeleton_ regionSkeleton = it->second->getRegionSkeleton();
			size_t count = regionSkeleton->count();
			for(size_t i = 0; i < count; i++){
				regionSkeleton->getNode(i)->center += transVec;
			}
	}


	/* 重新计算AABB */
	mGarment->requestRecomputeAABB();
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
