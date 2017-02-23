#include "ComponentForce.h"
#include <Mesh.h>

ComponentForce::ComponentForce(void)
{
}


ComponentForce::~ComponentForce(void)
{
}

Vec3d GravityForce::compute( size_t i, const Mesh_ mesh, const std::vector<Vec3d>& curPositions, const std::vector<Vec3d>& curVelocities, const std::vector<double> pointMass )
{
	return g * pointMass[i];
}

Vec3d StretchForce::compute( size_t index, const Mesh_ mesh, const std::vector<Vec3d>& curPositions, const std::vector<Vec3d>& curVelocities, const std::vector<double> pointMass )
{
	std::vector<size_t> neibours;
	for(Mesh::VertexVertexIter vv_it = mesh->vv_begin(Mesh::VertexHandle(index));
		vv_it.is_valid(); vv_it++){
			neibours.push_back(vv_it->idx());
	}
	Vec3d ret(0, 0, 0);
	for(size_t n = 0 ; n < neibours.size(); n++){
		size_t nei = neibours[n];				
		Vec3d curVec = curPositions[nei] - curPositions[index];
		double curLen = curVec.length();
		double restLen = (mesh->point(Mesh::VertexHandle(index))
			- mesh->point(Mesh::VertexHandle(nei))).length();
		ret += curVec.normalize_cond() * (curLen - restLen) * k;
	}
	return ret;
}
