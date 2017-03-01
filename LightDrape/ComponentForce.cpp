#include "ComponentForce.h"
#include "Mesh.h"
#include "Config.h"

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
	g = Vec3d(0, Config::getInstance()->gravity, 0);
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
	/*k = 0.5;*/
}

StretchForce::StretchForce( double k )
{
	/*this->k = k;*/
}

StretchForce::StretchForce( Mesh_ mesh ) : ComponentForce(mesh)
{
	/*k = 0.5;*/
	initSpring(mesh);
}

void StretchForce::setStiffness( double k )
{
	this->k = k;
}

void StretchForce::initSpring( Mesh_ mesh )
{
	mSprings.reserve(mesh->n_edges());
	Config_ config = Config::getInstance();
	for(auto e_it = mesh->edges_begin(); e_it != mesh->edges_end(); e_it++){
		Spring spring;
		Mesh::HalfedgeHandle he = mesh->halfedge_handle(*e_it, 0);
		spring.p1 = mesh->from_vertex_handle(he).idx();
		spring.p2 = mesh->to_vertex_handle(he).idx();
		spring.Ks = config->struct_spring_ks;
		spring.Kd = config->struct_spring_kd;
		spring.rest_length = (mesh->point(Mesh::VertexHandle(spring.p1)) - mesh->point(Mesh::VertexHandle(spring.p2))).length();
		mSprings.push_back(spring);
	}
}

DampForce::DampForce(void)
{
	mDamping = Config::getInstance()->damping_force;
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


void BendForce::compute( const std::vector<Vec3d>& curPositions, 
						const std::vector<Vec3d>& curVelocities, 
						const std::vector<double>& pointMass, 
						std::vector<Vec3d>& forces )
{
	size_t sprintCount = mSprings.size();
	for(size_t i = 0; i < sprintCount; i++){
		FaceAngleSpring& spring = mSprings[i];
		const Vec3d& ep1_vec = curPositions[spring.ep1];
		const Vec3d& ep2_vec = curPositions[spring.ep2];

		const Vec3d& fp1_vec = curPositions[spring.fp1];
		const Vec3d& fp2_vec = curPositions[spring.fp2];

		Vec3d n1 = (ep1_vec - fp1_vec) % (ep2_vec - fp1_vec);
		Vec3d n2 = (ep2_vec - fp2_vec) % (ep1_vec - fp2_vec);

		double cos_theta = (n1 | n2) / (n1.length() * n2.length());
		if(cos_theta < -1.0) cos_theta = -1.0;
		else if(cos_theta > 1.0) cos_theta = 1.0;

		double curAngle = 0;
		
		Vec3d f1e = fp1_vec - ep1_vec;
		if( (n2 | f1e ) < 0 ){
			curAngle = mPi + acos(cos_theta);
		}
		else
			curAngle = mPi - acos(cos_theta);

		double leftTerm = spring.Ks * (curAngle-spring.rest_angle);
		n1.normalize_cond();
		n2.normalize_cond();
		forces[spring.fp1] += n1 * leftTerm;
		forces[spring.fp2] += n2 * leftTerm;


		/* Damping forces */
		forces[spring.fp1] += n1 * (curVelocities[spring.fp1] | n1) * spring.Kd;
		forces[spring.fp2] += n2 * (curVelocities[spring.fp2] | n2) * spring.Kd;
	}
}

BendForce::BendForce( Mesh_ mesh ) : ComponentForce(mesh)
{
	mPi = 3.1415926;
	initSpring(mesh);	
}

BendForce::BendForce()
{
	mPi = 3.1415926;
}

void BendForce::initSpring( Mesh_ mesh )
{
	mSprings.reserve(mesh->n_edges());
	int cur = 0;
	Config_ config = Config::getInstance();
	for(auto e_it = mesh->edges_begin(); e_it != mesh->edges_end(); e_it++){
		if(cur == 131)
			std::cout << "haha";
		FaceAngleSpring spring;
		Mesh::HalfedgeHandle he1 = mesh->halfedge_handle(*e_it, 0);
		Mesh::HalfedgeHandle he2 = mesh->halfedge_handle(*e_it, 1);
		Mesh::FaceHandle f1 = mesh->face_handle(he1);
		Mesh::FaceHandle f2 = mesh->face_handle(he2);

		/* 边界边 */
		if(mesh->is_valid_handle(f1) == false || mesh->is_valid_handle(f2) == false)
			continue;

		spring.ep1 = mesh->from_vertex_handle(he1).idx();
		spring.ep2 = mesh->from_vertex_handle(he2).idx();

		for(auto fv_it = mesh->fv_begin(f1); fv_it.is_valid(); fv_it++){
			if(fv_it->idx() != spring.ep1 && fv_it->idx() != spring.ep2){
				spring.fp1 = fv_it->idx();
				break;
			}
		}

		for(auto fv_it = mesh->fv_begin(f2); fv_it.is_valid(); fv_it++){
			if(fv_it->idx() != spring.ep1 && fv_it->idx() != spring.ep2){
				spring.fp2 = fv_it->idx();
				break;
			}
		}

		spring.Ks = config->bend_spring_ks;
		spring.Kd = config->bend_spring_kd;

		const Vec3d& ep1_vec = mesh->point(Mesh::VertexHandle(spring.ep1));
		const Vec3d& ep2_vec = mesh->point(Mesh::VertexHandle(spring.ep2));

		const Vec3d& fp1_vec = mesh->point(Mesh::VertexHandle(spring.fp1));
		const Vec3d& fp2_vec = mesh->point(Mesh::VertexHandle(spring.fp2));

		Vec3d n1 = (ep1_vec - fp1_vec) % (ep2_vec - fp1_vec);
		Vec3d n2 = (ep2_vec - fp2_vec) % (ep1_vec - fp2_vec);
		double n1len = n1.length();
		double n2len = n2.length();
		double dot = (n1 | n2);
		double n1n2 = (n1.length() * n2.length());
		double the = (n1 | n2) / (n1.length() * n2.length());
		double aco = acos((n1 | n2) / (n1.length() * n2.length()));
		double cos_theta = (n1 | n2) / (n1.length() * n2.length());
		if (cos_theta < -1.0) cos_theta = -1.0 ;
		else if (cos_theta > 1.0) cos_theta = 1.0 ;
		Vec3d f1e = fp1_vec - ep1_vec;
		if( (n2 | f1e ) < 0 ){
			spring.rest_angle = mPi + acos(cos_theta);
		}
		else
			spring.rest_angle = mPi - acos(cos_theta);

		/* 调整两个面的向量的方向 */
// 		Vec3d f1e = fp1_vec - ep1_vec;
// 		if( (n2 | f1e ) < 0 ){
// 			std::swap(spring.ep1, spring.ep2);
// 		}

		mSprings.push_back(spring);
		cur++;
	}
}
