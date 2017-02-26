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

void GravityForce::compute(
	const std::vector<Vec3d>& curPositions,
	const std::vector<Vec3d>& curVelocities,
	const std::vector<double>& pointMass,
	std::vector<Vec3d>& forces)
{
	size_t pointCount = forces.size();
	for(size_t i = 0; i < pointCount; i++){
		forces[i] += g;
	}
}

GravityForce::GravityForce( Mesh_ mesh ) : ComponentForce(mesh)
{

}

GravityForce::GravityForce()
{
	g = Vec3d(0, -0.00981, 0);
}
void StretchForce::compute(
	const std::vector<Vec3d>& curPositions,
	const std::vector<Vec3d>& curVelocities,
	const std::vector<double>& pointMass,
	std::vector<Vec3d>& forces)
{
	//add spring forces
	for(size_t i = 0; i < mSprings.size(); i++) {
		Vec3d p1 = curPositions[mSprings[i].p1];
		Vec3d p2 = curPositions[mSprings[i].p2];
		Vec3d v1 = curVelocities[mSprings[i].p1];
		Vec3d v2 = curVelocities[mSprings[i].p2];
		Vec3d deltaP = p1-p2;
		Vec3d deltaV = v1-v2;
		float dist = deltaP.length();

		float leftTerm = -mSprings[i].Ks * (dist-mSprings[i].rest_length);
		float rightTerm = mSprings[i].Kd * (deltaV|deltaP/dist);
		Vec3d springForce = deltaP.normalize_cond() * (leftTerm + rightTerm);
		forces[mSprings[i].p1] += springForce;
		forces[mSprings[i].p2] -= springForce;
	}

}

StretchForce::StretchForce()
{
	k = 0.5;
}

StretchForce::StretchForce( double k )
{
	this->k = k;
}

StretchForce::StretchForce( Mesh_ mesh ) : ComponentForce(mesh)
{
	k = 0.5;
	initSpring(mesh);
}

void StretchForce::setStiffness( double k )
{
	this->k = k;
}

void StretchForce::initSpring( Mesh_ mesh )
{
	mSprings.reserve(mesh->n_edges());
	for(auto e_it = mesh->edges_begin(); e_it != mesh->edges_end(); e_it++){
		Spring spring;
		Mesh::HalfedgeHandle he = mesh->halfedge_handle(*e_it, 0);
		spring.p1 = mesh->from_vertex_handle(he).idx();
		spring.p2 = mesh->to_vertex_handle(he).idx();
		spring.Ks = 1.5;
		spring.Kd = -0.75;
		spring.rest_length = (mesh->point(Mesh::VertexHandle(spring.p1)) - mesh->point(Mesh::VertexHandle(spring.p2))).length();
		mSprings.push_back(spring);
	}
}

DampForce::DampForce(void)
{
	mDamping = -0.0125;
}

DampForce::DampForce( double damping )
{
	mDamping = damping;
}


DampForce::~DampForce(void)
{
}

void DampForce::compute( const std::vector<Vec3d>& curPositions,
						const std::vector<Vec3d>& curVelocities,
						const std::vector<double>& pointMass,
						std::vector<Vec3d>& forces )
{
	size_t pointCount = forces.size();
	for(size_t i = 0; i < pointCount; i++){
		forces[i] += curVelocities[i] * mDamping;
	}
}

