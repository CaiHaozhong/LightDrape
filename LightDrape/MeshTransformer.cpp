#include "MeshTransformer.h"
#include "Vec3d.h"

MeshTransformer::MeshTransformer(void)
{
}

MeshTransformer::MeshTransformer( Mesh_ mesh )
{
	this->mMesh = mesh;
}


MeshTransformer::~MeshTransformer(void)
{
}

void MeshTransformer::setMesh( Mesh_ mesh )
{
	this->mMesh = mesh;
}

// 
// void MeshTransformer::transform()
// {
// 	Vec3d t = Vec3d(1.6854022400000002, -0.011820000000000164, 0.046564950000000022);
// 	for(Mesh::VertexIter it = mMesh->vertices_begin();
// 		it != mMesh->vertices_end(); it++){
// 			Vec3d& p = mMesh->point(*it);
// 			p += t;
// 	}
// }

void BackTransformer::transform()
{
	for(Mesh::VertexIter it = mMesh->vertices_begin();
		it != mMesh->vertices_end(); it++){
			Vec3d& point = mMesh->point(*it);
			point.values_[0] = -point.values_[0];
	}
}

void LeftTransformer::transform()
{
	for(Mesh::VertexIter it = mMesh->vertices_begin();
		it != mMesh->vertices_end(); it++){
			Vec3d& point = mMesh->point(*it);
			double x = point.values_[0];
			point.values_[0] = point.values_[2];
			point.values_[2] = -x;
	}
}

void RightTransformer::transform()
{
	for(Mesh::VertexIter it = mMesh->vertices_begin();
		it != mMesh->vertices_end(); it++){
			Vec3d& point = mMesh->point(*it);
			double x = point.values_[0];
			point.values_[0] = -point.values_[2];
			point.values_[2] = x;
	}
}

void RightFrontTransformer::transform()
{
	for(Mesh::VertexIter it = mMesh->vertices_begin();
		it != mMesh->vertices_end(); it++){
			Vec3d& point = mMesh->point(*it);
			double x = point.values_[0];
			double z = point.values_[2];
			double fac = 0.707106;
			point.values_[0] = fac * (x - z);
			point.values_[2] = fac * (x + z);
	}
}

void LeftFrontTransformer::transform()
{
	for(Mesh::VertexIter it = mMesh->vertices_begin();
		it != mMesh->vertices_end(); it++){
			Vec3d& point = mMesh->point(*it);
			double x = point.values_[0];
			double z = point.values_[2];
			double fac = 0.707106;
			point.values_[0] = fac * (x + z);
			point.values_[2] = fac * (z - x);
	}
}

MeshTransformer_ AxisYTransFactory::createTransfomer( std::string type )
{
	if(type == "back"){
		return smartNew(BackTransformer);
	}
	else if(type == "left_front"){
		return smartNew(LeftFrontTransformer);
	}
	else if(type == "right_front"){
		return smartNew(RightFrontTransformer);
	}
	else if(type == "left"){
		return smartNew(LeftTransformer);
	}
	else if(type == "right"){
		return smartNew(RightTransformer);
	}
	return nullptr;
}
