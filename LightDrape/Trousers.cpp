#include "Trousers.h"
#include "Human.h"


Trousers::~Trousers(void)
{
}

size_t Trousers::getGeodesicSource()
{
	/* TO DO */
	return WatertightMesh::getGeodesicSource();
}

Vec3d Trousers::getAlignPoint()
{
	Segment_ seg = this->getSegment();
	if(seg == nullptr){
		PRINTLN("In getAlignPoint of human. Segment human first.");
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

	/* 计算y值，位于大腿顶端 */
	std::set<size_t> vers = torse->getVertices();
	Mesh_ mesh = torse->getMesh();
	y = 10000;
	for(auto it = vers.begin(); it != vers.end(); it++){
		Vec3d& p = mesh->point(Mesh::VertexHandle(*it));
		if(p.values_[1] < y){
			y = p.values_[1];
		}
	}
	return Vec3d(x,y,z);
}

double Trousers::dressHeight( Human_ human )
{
	return 8.5;
	//return human->getLegLength() - 1;
}

Trousers::Trousers( Mesh_ mesh ) :Garment(mesh)
{

}
