#include "ComponentForce.h"
#include <Mesh.h>

ComponentForce::ComponentForce(void)
{
	mMesh = nullptr;
}

ComponentForce::ComponentForce( Mesh_ mesh )
{
	mMesh = mesh;
}


ComponentForce::~ComponentForce(void)
{
}

Mesh_ ComponentForce::setMesh() const
{
	return mMesh;
}

void ComponentForce::getMesh( Mesh_ val )
{
	mMesh = val;
}

Vec3d GravityForce::compute( size_t i, const std::vector<Vec3d>& curPositions, const std::vector<Vec3d>& curVelocities, const std::vector<double> pointMass )
{
	return g * pointMass[i];
}

GravityForce::GravityForce( Mesh_ mesh ) : ComponentForce(mesh)
{

}

GravityForce::GravityForce()
{
	g = Vec3d(0, -9.8, 0);
}
Vec3d StretchForce::compute( size_t index, const std::vector<Vec3d>& curPositions, const std::vector<Vec3d>& curVelocities, const std::vector<double> pointMass )
{
	std::vector<size_t> neibours;
	for(Mesh::VertexVertexIter vv_it = mMesh->vv_begin(Mesh::VertexHandle(index));
		vv_it.is_valid(); vv_it++){
			neibours.push_back(vv_it->idx());
	}
	Vec3d ret(0, 0, 0);
	for(size_t n = 0 ; n < neibours.size(); n++){
		size_t nei = neibours[n];				
		Vec3d curVec = curPositions[nei] - curPositions[index];
		double curLen = curVec.length();
		double restLen = (mMesh->point(Mesh::VertexHandle(index))
			- mMesh->point(Mesh::VertexHandle(nei))).length();
		ret += curVec.normalize_cond() * (curLen - restLen) * k;
	}
	return ret;
}

StretchForce::StretchForce()
{
	k = 10000;
}

StretchForce::StretchForce( double k )
{
	this->k = k;
}

StretchForce::StretchForce( Mesh_ mesh ) : ComponentForce(mesh)
{

}

void StretchForce::setStiffness( double k )
{
	this->k = k;
}
