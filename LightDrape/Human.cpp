#include "SkeletonizationPreCompile.h"
#include "Human.h"
#include "HumanSegmenter.h"
#include "ClothSegmenter.h"
#include "GarmentFitter.h"
#include "LaplacianGlobalDeformer.h"
#include "LaplacianLocalDeformer.h"
#include "UglyDeformer.h"

Human::~Human(void)
{
}

Human::Human( Mesh_ mesh ) :WatertightMesh(mesh)
{

}

size_t Human::getGeodesicSource()
{
	/* TO DO */
	return WatertightMesh::getGeodesicSource();
}

void Human::dress( Garment_ garment )
{
	/* 先各自进行骨骼提取 */
	MeshSkeletonization_ skeletonizer = smartNew(MeshSkeletonizationCached);
	PRINTLN("Skeletonize human...");
	skeletonizer->skeletonize(shared_from_this());
	PRINTLN("Skeletonize human end.");
	PRINTLN("Skeletonize garment...");
	skeletonizer->skeletonize(garment);
	PRINTLN("Skeletonize garment end.");
	this->dumpSkeleton(getName());
	garment->dumpSkeleton(garment->getName());

	/* 各自进行分割 */
	MeshSegmenter_ humanSegmenter = std::make_shared<HumanSegmenter>(shared_from_this());
	PRINTLN("Segment human...");
	humanSegmenter->segment();
	PRINTLN("Segment human end.");
	MeshSegmenter_ clothSegmenter = std::make_shared<ClothSegmenter>(garment);
	PRINTLN("Segment garment...");
	clothSegmenter->segment();
	PRINTLN("Segment garment end.");
	
	GarmentFitter_ fitter = std::make_shared<GarmentFitter>(garment);
	fitter->setMeshDeformer(smartNew(LaplacianGlobalDeformer));
	PRINTLN("Fitting garment...");
	fitter->fit(shared_from_this());
	PRINTLN("Fitting garment end.");
}

Vec3d Human::getAlignPoint()
{
	Segment_ seg = this->getSegment();
	if(seg == nullptr){
		PRINTLN("In getAlignPoint of human. Segment human first.");
		return WatertightMesh::getAlignPoint();
	}
	std::vector< std::pair<int, Region_> >& regions = seg->getRegionsRaw();
	size_t count = regions.size();
	Vec3d ret = Vec3d(0,0,0);
	for(size_t i = 0; i < count; i++){
		std::pair<int, Region_>& reg = regions[i];
		if(reg.first == Segment::BODY_LEFT_HAND
			|| reg.first == Segment::BODY_RIGHT_HAND){
// 			size_t startSke = reg.second->getStart();
// 			ret += getSkeleton()->nodeAt(startSke)->point;
				RegionSkeleton_ rs = reg.second->getRegionSkeleton();
				ret += rs->getNode(0)->center;
			/*ret += reg.second->getRegionSkeleton()->start()->center;*/
// 			RegionSkeleton_ rs = reg.second->getRegionSkeleton();
// 			for(size_t r = 0; r < rs->count(); r++){
// 				RegionSkeletonNode_ node = rs->getNode(r);
// 				if(node->vers.size() > 10){
// 					ret += node->center;
// 					break;
// 				}
// 			}
		}
	}
	return ret/2.0;
}
