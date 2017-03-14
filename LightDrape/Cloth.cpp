#include "Cloth.h"
#include "Human.h"
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
// 		if(reg.first == ClothSegment::CLOTH_LEFT_SLEEVE
// 			|| reg.first == ClothSegment::CLOTH_RIGHT_SLEEVE){
// //  				size_t startSke = reg.second->getStart();
// //  				ret += getSkeleton()->nodeAt(startSke)->point;
// 				RegionSkeleton_ rs = reg.second->getRegionSkeleton();
// 				ret += rs->getNode(0)->center;
// // 				for(size_t r = 0; r < rs->count(); r++){
// // 					RegionSkeletonNode_ node = rs->getNode(r);
// // 					if(node->vers.size() > 10){
// // 						ret += node->center;
// // 						break;
// // 					}
// // 				}
// 				//ret += reg.second->getRegionSkeleton()->start()->center;
// 		}
// 	}
// //	ret.values_[1] -= this->getHeight() * 0.35;
// 	return ret/2.0;
	Segment_ seg = this->getSegment();
	if(seg == nullptr){
		PRINTLN("In getAlignPoint of Cloth. Segment Cloth first.");
		return WatertightMesh::getAlignPoint();
	}
	Region_ torse = seg->getMatch(Segment::BODY_TORSE);
	double x = 0, y = 0, z = 0;
	RegionSkeleton_ regionSke = torse->getRegionSkeleton();
	for(size_t i = 0; i < regionSke->count(); i++){
		Vec3d& center = regionSke->getNode(i)->center;
		x += center.values_[0];
		z += center.values_[2];
	}
	x /= regionSke->count();
	z /= regionSke->count();
	y = this->getMax().values_[1] - 2; //衣服比肩膀高两厘米
	return Vec3d(x,y,z);
}

double Cloth::dressHeight( Human_ human )
{
	return human->getShoulderHeight();
}
