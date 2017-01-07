#include "GarmentFitter.h"
#include "Garment.h"
#include "Human.h"
#include "Segment.h"
#include "MeshDeformer.h"

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
	if(human == nullptr){
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
	mGarment->dumpSkeleton("beforeFit");
	std::vector<std::pair<int, Region_> > regions = humSeg->getRegionsRaw();
	size_t count = regions.size();
	for(size_t i = 0; i < count; i++){
		if(regions[i].first == Segment::BODY_TORSE)
			continue;
		Region_ garmentRegion = garSeg->getMatch(regions[i].first);
		if(garmentRegion != nullptr){
			fit(garmentRegion, regions[i].second);
			if(regions[i].first == Segment::BODY_LEFT_HAND){
				garmentRegion->dump("leftSleeve");
				regions[i].second->dump("leftHand");
			}
			else if(regions[i].first == Segment::BODY_RIGHT_HAND){
				garmentRegion->dump("rightSleeve");
				regions[i].second->dump("rightHand");
			}
			else if(regions[i].first == Segment::BODY_TORSE){
				garmentRegion->dump("garTor");
				regions[i].second->dump("bodyTor");
			}
		}
	}

	for(size_t i = 0; i < mGarSkeTrans.size(); i++){
		std::pair<size_t, Vec3d>& trans = mGarSkeTrans[i];
		mGarment->getSkeleton()->nodeAt(trans.first)->point += trans.second;
	}
	mGarment->dumpSkeleton("afterFit");
	mHuman->dumpSkeleton("human");
	alterGarment();
	if(mMeshDeformer == nullptr){
		PRINTLN("No avalible meshdeformer.");
		return;
	}
	mMeshDeformer->deformMesh(mGarment);	
}

void GarmentFitter::fit(Region_ garmentRegion, Region_ humanRegion )
{
	if(garmentRegion == nullptr || humanRegion == nullptr){
		PRINTLN("In fit, regions should not be null.");
		return;
	}		
	Skeleton_ garSkeleton = mGarment->getSkeleton();
	Skeleton_ humSkeleton = mHuman->getSkeleton();
	std::vector<size_t> garOrderedSkes;
	std::vector<size_t> humOrderSkes;
	garmentRegion->toSortedSkeleton(garOrderedSkes, std::make_shared<ChooseLongBranch>(garSkeleton));
	humanRegion->toSortedSkeleton(humOrderSkes, std::make_shared<ChooseLongBranch>(humSkeleton));
	size_t garStartSke = garOrderedSkes[0];
	size_t humStartSke = humOrderSkes[0];
	skeTranslateTo(garStartSke, humSkeleton->nodeAt(humStartSke)->point);
	double garLen = 0;
	double humLen = 0;
	size_t humCurIndex = 0;
	double cachedLen = 0;
	for(size_t i = 1; i < garOrderedSkes.size(); i++){
		size_t curSke = garOrderedSkes[i];
		garLen += garSkeleton->nodeEuclideanDis(garOrderedSkes[i-1], curSke);		
		while(humLen < garLen){
			humCurIndex += 1;
			if(humCurIndex >= humOrderSkes.size())
				break;
			cachedLen = humSkeleton->nodeEuclideanDis(humOrderSkes[humCurIndex-1],
				humOrderSkes[humCurIndex]);
			humLen += cachedLen;
		}
		if(humCurIndex >= humOrderSkes.size()) // 假设袖子比手臂短
			break;
		Vec3d hp1 = humSkeleton->nodeAt(humOrderSkes[humCurIndex-1])->point;
		Vec3d hp2 = humSkeleton->nodeAt(humOrderSkes[humCurIndex])->point;
		Vec3d transVec = hp2 + (hp1-hp2) * (humLen-garLen)/cachedLen;
		skeTranslateTo(curSke, transVec);
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
	mGarment->translate(transVec);
	Skeleton_ skeleton = mGarment->getSkeleton();
	Skeleton::NodeList nodes = skeleton->getNodeList();
	for(size_t i = 0; i < nodes.size(); i++){
		nodes[i]->point += transVec;
	}
}

void GarmentFitter::skeTranslateTo( size_t ske, Vec3d dest )
{
	if(mGarment == nullptr){
		PRINTLN("In skeTranslateTo(). Garment is null");
		return ;
	}
	Vec3d& skeP = mGarment->getSkeleton()->nodeAt(ske)->point;
	mGarSkeTrans.push_back(std::make_pair(ske, dest-skeP));
}

void GarmentFitter::setGarment( Garment_ garment )
{
	this->mGarment = garment;
}

void GarmentFitter::alterGarment()
{
	size_t count = mGarSkeTrans.size();
	Skeleton_ skeleton = mGarment->getSkeleton();
	bool* hasAlter = new bool[mGarment->n_vertices()];
	memset(hasAlter, 0, mGarment->n_vertices());
	for(size_t i = 0; i < count; i++){
		std::pair<size_t, Vec3d>& alter = mGarSkeTrans[i];
		std::vector<size_t>& corrVertices 
			= skeleton->nodeAt(alter.first)->correspondanceIndices;
		size_t corCount = corrVertices.size();
		for(size_t c = 0; c < corCount; c++){
			size_t v = corrVertices[c];
			if(hasAlter[v]){
				PRINTLN("single vertex corrspondent to multi skenode");
				continue;
			}
			mGarment->alterVertex(v, alter.second);
			hasAlter[v] = true;
		}
	}
	delete [] hasAlter;
}

void GarmentFitter::setMeshDeformer( MeshDeformer_ meshDeformer )
{
	this->mMeshDeformer = meshDeformer;
}
