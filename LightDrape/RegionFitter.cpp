#include "RegionFitter.h"
#include "Region.h"
#include "Mesh.h"
#include "VertexAlter.h"
#include "LevelSet.h"
#include "Quaternion.h"
RegionFitter::RegionFitter(void)
{
}


RegionFitter::~RegionFitter(void)
{
}

RegionFitter::RegionFitter(Region_ garmentRegion){
	this->mGarmentRegion = garmentRegion;
}

void RegionFitter::setGarmentRegion(Region_ garmentRegion){
	this->mGarmentRegion = garmentRegion;
}

void RegionFitter::rotateCircle( VertexAlter_ alter, const Mesh_ mesh, const std::vector<size_t>& circle, const Vec3d& center, const Vec3d& axis, double angle )
{
	if(alter == nullptr || mesh == nullptr){
		return;
	}
	size_t count = circle.size();
	Quaternion quaternion(axis, angle);
	for(size_t i = 0; i < count; i++){
		size_t ver = circle[i];
		const Vec3d& point = mesh->point(Mesh::VertexHandle(ver));
		Vec3d rpoint = point;
		rotatePoint(rpoint, center, quaternion);
		alter->add(ver, rpoint - point);
	}
}

void RegionFitter::rotateCircle( VertexAlter_ alter, const Mesh_ mesh, const LevelCircle_ circle, const Vec3d& center, const Vec3d& axis, double angle )
{
	if(alter == nullptr || mesh == nullptr){
		return;
	}
	std::vector<LevelNode_> nodes = circle->levelNodes;
	std::vector<size_t> vertices;
	size_t count = nodes.size();
	for(size_t i = 0; i < count; i++){
		vertices.push_back(nodes[i]->getNearestVertex(mesh));
	}
	rotateCircle(alter, mesh, vertices, center, axis, angle);
}

void RegionFitter::rotatePoint( Vec3d& point, const Vec3d& center, Quaternion& quaternion)
{
	point -= center;
	quaternion.rotate(point);
	point += center;
}
