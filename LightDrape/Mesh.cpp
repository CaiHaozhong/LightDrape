#include "Mesh.h"

Mesh::Mesh() :_Mesh(){
	mHasRequestAABB = false;
	float maxFloat = std::numeric_limits<float>::max();
	float minFloat = std::numeric_limits<float>::min();
	mMinPoint = OpenMesh::Vec3f(maxFloat,maxFloat,maxFloat);
	mMaxPoint = OpenMesh::Vec3f(minFloat,minFloat,minFloat);
}

Mesh::~Mesh() {
}

void Mesh::requestRecomputeAABB(){
	mHasRequestAABB = false;
	requestAABB();
}

void Mesh::requestAABB(){
	if(mHasRequestAABB)
		return;
	for(Mesh::ConstVertexIter vIt = this->vertices_begin(); vIt != this->vertices_end(); vIt++){
		const Mesh::Point& p = this->point(*vIt);
		mMinPoint.minimize(OpenMesh::vector_cast<OpenMesh::Vec3d>(p));
		mMaxPoint.maximize(OpenMesh::vector_cast<OpenMesh::Vec3d>(p));
	}
	mHasRequestAABB = true;
}

Vec3d Mesh::getMin(){
	return mMinPoint;
}

Vec3d Mesh::getMax(){
	return mMaxPoint;
}

double Mesh::getDigonalLen(){
	return (mMaxPoint-mMinPoint).length();
}

double Mesh::getWidth(){
	return mMaxPoint.values_[0] - mMinPoint.values_[0];
}

double Mesh::getHeight(){
	return mMaxPoint.values_[1] - mMinPoint.values_[1];
}

double Mesh::getLength(){
	return mMaxPoint.values_[2] - mMinPoint.values_[2];
}

