#include "Cloth.h"
#include "ClothSegment.h"

Cloth::~Cloth(void)
{
}

Cloth::Cloth( Mesh_ mesh ) :Garment(mesh)
{

}

size_t Cloth::getGeodesicSource()
{
	/* TO DO */
	return WatertightMesh::getGeodesicSource();
}

Vec3d Cloth::getAlignPoint()
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
		if(reg.first == ClothSegment::CLOTH_LEFT_SLEEVE
			|| reg.first == ClothSegment::CLOTH_RIGHT_SLEEVE){
// 				size_t startSke = reg.second->getStart();
// 				ret += getSkeleton()->nodeAt(startSke)->point;
				RegionSkeleton_ rs = reg.second->getRegionSkeleton();
				for(size_t r = 0; r < rs->count(); r++){
					RegionSkeletonNode_ node = rs->getNode(r);
					if(node->vers.size() > 10){
						ret += node->center;
						break;
					}
				}
				//ret += reg.second->getRegionSkeleton()->start()->center;
		}
	}
//	ret.values_[1] -= this->getHeight() * 0.35;
	return ret/2.0;
}
