#include "SkeletonizationPreCompile.h"
#include "Human.h"
#include "Garment.h"
#include "HumanSegmenter.h"
#include "ClothSegmenter.h"
#include "TrousersSegmenter.h"
#include "GarmentFitter.h"
#include "LaplacianGlobalDeformer.h"
#include "LaplacianLocalDeformer.h"
#include "UglyDeformer.h"
#include "HumanFeature.h"
#include "GarmentPenetrationResolver.h"
#include "GarmentPhysicalSimulator.h"
#include "FrameToOBJFileWriter.h"
#include "MeshSegmentListener.h"
#include "Config.h"
#include <thread>
Human::~Human(void)
{
}

Human::Human( Mesh_ mesh ) :WatertightMesh(mesh)
{
	mFeature = nullptr;
	mSimulator = smartNew(GarmentPhysicalSimulator);
}

size_t Human::getGeodesicSource()
{
	/* TO DO */
	//return 12478;
	return WatertightMesh::getGeodesicSource();
}

void Human::dress( Garment_ garment )
{
	mGarment = garment;
// 	/* 先各自进行骨骼提取 */
// 	MeshSkeletonization_ skeletonizer = smartNew(MeshSkeletonization);
// 	PRINTLN("Skeletonize human...");
// 	skeletonizer->skeletonize(shared_from_this());
// 	PRINTLN("Skeletonize human end.");
// 	PRINTLN("Skeletonize garment...");
// 	skeletonizer->skeletonize(garment);
// 	PRINTLN("Skeletonize garment end.");
// 
//  	/* 各自进行分割 */
// 	MeshSegmenter_ humanSegmenter = std::make_shared<HumanSegmenter>(shared_from_this());
// 	humanSegmenter->addSegmentListener(mMeshSegmentListeners);
// 	PRINTLN("Segment human...");
// 	humanSegmenter->segment();
// 	PRINTLN("Segment human end.");
// 	MeshSegmenter_ clothSegmenter = nullptr;
// 	Config_ config = Config::getInstance();
// 	std::string clothType = config->clothType;
// 	if(clothType == "cloth"){
// 		clothSegmenter = std::make_shared<ClothSegmenter>(garment);
// 	}
// 	else if(clothType == "trousers"){
// 		clothSegmenter = std::make_shared<TrousersSegmenter>(garment);
// 	}
// 	if(clothSegmenter == nullptr){
// 		PRINTLN("Human::dress( Garment_ garment ), Error cloth type!");
// 		return;
// 	}
// 	clothSegmenter->addSegmentListener(mMeshSegmentListeners);
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
// 
// 
// 	/* 穿透调整 */
// // 	GarmentPenetrationResolver_ penetrationResolver = smartNew(GarmentPenetrationResolver);
// // 	penetrationResolver->setGarment(garment->getOriginalMesh());
// // 	penetrationResolver->setHuman(this->getOriginalMesh());
// // 	bool isSuc = penetrationResolver->resolve();
// // 	if(isSuc){
// // 		PRINTLN("Resolve Penetration successfully.");
// // 	}
// // 	else{
// // 		PRINTLN("Resolve Penetration fail.");
// // 	}
 	std::thread t(&Human::doSimulate, this, garment);
 	t.detach();
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
	y = mGarment->dressHeight(shared_from_this());
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

void Human::doSimulate(Garment_ garment)
{
	/* 物理模拟 */
	mSimulator->initWithGarmentAndHuman(garment->getOriginalMesh(), this->getOriginalMesh());
	std::vector<size_t> fixPoints;
	garment->getSimulateFixPoints(fixPoints);
	mSimulator->addGarmentFixPoints(fixPoints);
	mSimulator->simulate();	
}

void Human::addGarmentSimulationCallBack( GarmentSimulationCallBack_ callBack )
{
	mSimulator->addGarmentSimulationCallBack(callBack);
}

void Human::addMeshSegmentListener( MeshSegmentListener_ listener )
{
	mMeshSegmentListeners.push_back(listener);
}

double Human::getUpperLegWidth()
{
	return mFeature->upperLegWidth;
}

double Human::getUnderBellyHeight()
{
	return mFeature->underbellyHeight;
}

double Human::getClothHeight()
{
	return mFeature->clothHeight;
}

double Human::getTrouserHeight()
{
	return mFeature->trousersHeight;
}


