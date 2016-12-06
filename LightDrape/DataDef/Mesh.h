#pragma once
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <climits>
#include "Skeleton.h"
typedef OpenMesh::TriMesh_ArrayKernelT<> _Mesh;

/*
 * 具有包围盒的三角网格
 */
class Mesh : public _Mesh {
public:

	/* Mesh Type */
	static const int TYPE_HUMAN = 0;
	static const int TYPE_CLOTH = 1;
	static const int TYPE_PANT = 2;
	static const int TYPE_SKIRT = 3;
	static const int TYPE_UNKNOWN = 4;

	Mesh();

	~Mesh();

	/** 当模型变形之后，需要重新计算包围盒，则调用这个 **/
	void requestRecomputeAABB();

	/** 计算包围盒 **/
	void requestAABB();	

	OpenMesh::Vec3f getMin();

	OpenMesh::Vec3f getMax();

	/** 获取包围盒对角线的长度 **/
	float getDigonalLen();

	float getHeight();

	int getType() const { return mType; }

	void setType(int val) { mType = val; }

	Skeleton* getSkeleton() const { return mSkeleton; }

	void setSkeleton(Skeleton* val) { mSkeleton = val; }
private:
	bool mHasRequestAABB;

	OpenMesh::Vec3f mMinPoint, mMaxPoint;

	int mType;

	Skeleton* mSkeleton;

};
