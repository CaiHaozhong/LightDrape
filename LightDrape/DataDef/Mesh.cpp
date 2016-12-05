#include "Mesh.h"

Mesh::Mesh() :_Mesh(){
	mHasRequestAABB = false;
	float maxFloat = std::numeric_limits<float>::max();
	float minFloat = std::numeric_limits<float>::min();
	mMinPoint = OpenMesh::Vec3f(maxFloat,maxFloat,maxFloat);
	mMaxPoint = OpenMesh::Vec3f(minFloat,minFloat,minFloat);
	mType = TYPE_UNKNOWN;
	mSkeleton = nullptr;
}

Mesh::~Mesh() {
	if(mSkeleton != nullptr){
		delete mSkeleton;
		mSkeleton = nullptr;
	}
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
		mMinPoint.minimize(OpenMesh::vector_cast<OpenMesh::Vec3f>(p));
		mMaxPoint.maximize(OpenMesh::vector_cast<OpenMesh::Vec3f>(p));
	}
	mHasRequestAABB = true;
}

OpenMesh::Vec3f Mesh::getMin(){
	return mMinPoint;
}

OpenMesh::Vec3f Mesh::getMax(){
	return mMaxPoint;
}

float Mesh::getDigonalLen(){
	return (mMaxPoint-mMinPoint).length();
}

float Mesh::getHeight(){
	return mMaxPoint.values_[1] - mMinPoint.values_[1];
}

