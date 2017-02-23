#include "SkeletonizationPreCompile.h"
#include "Human.h"
#include "HumanSegmenter.h"
#include "ClothSegmenter.h"
#include "GarmentFitter.h"
#include "LaplacianGlobalDeformer.h"
#include "LaplacianLocalDeformer.h"
#include "UglyDeformer.h"
#include "HumanFeature.h"
#include "PenetrationResolver.h"

Human::~Human(void)
{
}

Human::Human( Mesh_ mesh ) :WatertightMesh(mesh)
{
	mFeature = nullptr;
}

size_t Human::getGeodesicSource()
{
	/* TO DO */
	return WatertightMesh::getGeodesicSource();
}

void Human::dress( Garment_ garment )
{
// 	/* 先各自进行骨骼提取 */
// 	MeshSkeletonization_ skeletonizer = smartNew(MeshSkeletonizationCached);
// 	PRINTLN("Skeletonize human...");
// 	skeletonizer->skeletonize(shared_from_this());
// 	PRINTLN("Skeletonize human end.");
// 	PRINTLN("Skeletonize garment...");
// 	skeletonizer->skeletonize(garment);
// 	PRINTLN("Skeletonize garment end.");
// 	this->dumpSkeleton(getName());
// 	garment->dumpSkeleton(garment->getName());
// 
// 	/* 各自进行分割 */
// 	MeshSegmenter_ humanSegmenter = std::make_shared<HumanSegmenter>(shared_from_this());
// 	PRINTLN("Segment human...");
// 	humanSegmenter->segment();
// 	PRINTLN("Segment human end.");
// 	MeshSegmenter_ clothSegmenter = std::make_shared<ClothSegmenter>(garment);
// 	PRINTLN("Segment garment...");
// 	clothSegmenter->segment();
// 	PRINTLN("Segment garment end.");
// 	
// 	GarmentFitter_ fitter = std::make_shared<GarmentFitter>(garment);
// 	fitter->setMeshDeformer(smartNew(LaplacianGlobalDeformer));
// 	PRINTLN("Fitting garment...");
// 	fitter->fit(shared_from_this());
// 	PRINTLN("Fitting garment end.");
// 
// 	/* 修改原始网格 */
// 	garment->alterOriginalMesh();

// 	/* 穿透调整 */
// 	PenetrationResolver_ penetrationResolver = smartNew(PenetrationResolver);
// 	penetrationResolver->setGarmentMesh(garment);
// 	penetrationResolver->setHumanMesh(getOriginalMesh());
// 	bool isSuc = penetrationResolver->resolve();
// 	if(isSuc){
// 		PRINTLN("Resolve Penetration successfully.");
// 	}
// 	else{
// 		PRINTLN("Resolve Penetration fail.");
// 	}

	
}

Vec3d Human::getAlignPoint()
{
// 	Segment_ seg = this->getSegment();
// 	if(seg == nullptr){
// 		PRINTLN("In getAlignPoint of human. Segment human first.");
// 		return WatertightMesh::getAlignPoint();
// 	}
// 	std::vector< std::pair<int, Region_> >& regions = seg->getRegionsRaw();
// 	size_t count = regions.size();
// 	Vec3d ret = Vec3d(0,0,0);
// 	for(size_t i = 0; i < count; i++){
// 		std::pair<int, Region_>& reg = regions[i];
// 		if(reg.first == Segment::BODY_LEFT_HAND
// 			|| reg.first == Segment::BODY_RIGHT_HAND){
// // 			size_t startSke = reg.second->getStart();
// // 			ret += getSkeleton()->nodeAt(startSke)->point;
// 				RegionSkeleton_ rs = reg.second->getRegionSkeleton();
// 				ret += rs->getNode(0)->center;
// 			/*ret += reg.second->getRegionSkeleton()->start()->center;*/
// // 			RegionSkeleton_ rs = reg.second->getRegionSkeleton();
// // 			for(size_t r = 0; r < rs->count(); r++){
// // 				RegionSkeletonNode_ node = rs->getNode(r);
// // 				if(node->vers.size() > 10){
// // 					ret += node->center;
// // 					break;
// // 				}
// // 			}
// 		}
// 	}
// 	return ret/2.0;
	Segment_ seg = this->getSegment();
	if(seg == nullptr){
		PRINTLN("In getAlignPoint of human. Segment human first.");
		return WatertightMesh::getAlignPoint();
	}
	Region_ torse = nullptr;
	std::vector< std::pair<int, Region_> >& regions = seg->getRegionsRaw();
	size_t count = regions.size();
	for(size_t i = 0; i < count; i++){
		std::pair<int, Region_>& reg = regions[i];
		if(reg.first == Segment::BODY_TORSE){
			torse = reg.second;
			break;
		}
	}
	if(torse == nullptr){
		PRINTLN("In getAlignPoint of human. Human no torse!");
		return WatertightMesh::getAlignPoint();
	}
	double x = 0, y = 0, z = 0;
	RegionSkeleton_ regionSke = torse->getRegionSkeleton();
	for(size_t i = 0; i < regionSke->count(); i++){
		Vec3d& center = regionSke->getNode(i)->center;
		x += center.values_[0];
		z += center.values_[2];
	}
	x /= regionSke->count();
	z /= regionSke->count();
	y = this->getShoulderHeight();
	return Vec3d(x,y,z);
}

HumanFeature_ Human::getFeature() const
{
	return mFeature;
}

void Human::setFeature( HumanFeature_ val )
{
	mFeature = val;
}

double Human::getHumanHeight()
{
	if(mFeature != nullptr){
		return mFeature->height;
	}
	return 0;
}

double Human::getShoulderHeight()
{
	if(mFeature != nullptr){
		return mFeature->shouldHeight;
	}
	return 0;
}

double Human::getLegLength()
{
	if(mFeature != nullptr){
		return mFeature->legLength;
	}
	return 0;
}
