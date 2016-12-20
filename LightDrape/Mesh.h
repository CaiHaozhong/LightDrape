#pragma once
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <climits>
#include "Vec3d.h"
#include "Common.h"

typedef OpenMesh::TriMesh_ArrayKernelT<> _Mesh;

/*
 * ���а�Χ�е���������
 */
class Mesh : public _Mesh {
public:
// 	/* Mesh Type */
// 	static const int TYPE_HUMAN = 0;
// 	static const int TYPE_CLOTH = 1;
// 	static const int TYPE_PANT = 2;
// 	static const int TYPE_SKIRT = 3;
// 	static const int TYPE_UNKNOWN = 4;

	Mesh();

	~Mesh();

	/** ��ģ�ͱ���֮����Ҫ���¼����Χ�У��������� **/
	void requestRecomputeAABB();

	/** �����Χ�� **/
	void requestAABB();	

	Vec3d getMin();

	Vec3d getMax();

	/** ��ȡ��Χ�жԽ��ߵĳ��� **/
	float getDigonalLen();

	/* x �� */
	float getWidth();

	/* y �� */
	float getHeight();

	/* z �� */
	float getLength();
	
private:
	bool mHasRequestAABB;

	Vec3d mMinPoint, mMaxPoint;

};

S_PTR(Mesh);