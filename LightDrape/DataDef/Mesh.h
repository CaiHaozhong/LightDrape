#pragma once
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <climits>
#include "Skeleton.h"
typedef OpenMesh::TriMesh_ArrayKernelT<> _Mesh;

/*
 * ���а�Χ�е���������
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

	/** ��ģ�ͱ���֮����Ҫ���¼����Χ�У��������� **/
	void requestRecomputeAABB();

	/** �����Χ�� **/
	void requestAABB();	

	OpenMesh::Vec3f getMin();

	OpenMesh::Vec3f getMax();

	/** ��ȡ��Χ�жԽ��ߵĳ��� **/
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
