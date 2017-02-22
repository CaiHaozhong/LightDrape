#pragma once
#include "Common.h"
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/graph_traits_Surface_mesh.h>
#include <CGAL/boost/graph/properties_Surface_mesh.h>
#include <CGAL/Mean_curvature_flow_skeletonization.h>
typedef CGAL::Simple_cartesian<double>                        Simple_cartesian_Kernel;
typedef CGAL::Surface_mesh<Simple_cartesian_Kernel::Point_3>                   CGAL_Surface_mesh;
typedef CGAL::Mean_curvature_flow_skeletonization<CGAL_Surface_mesh> Skeletonization;
typedef Skeletonization::Skeleton CGALSkeleton;
/* ��Mesh���й�����ȡ������ȡ���Ĺ��������Mesh���󣬽�������->�����ڵ㡱��
 * ���Դ����Mesh����
 * Ҫ��MeshΪWatertightMesh
 */
class WatertightMesh;
class Skeleton;
S_PTR(WatertightMesh);
S_PTR(Skeleton);
class MeshSkeletonization
{
public:
	MeshSkeletonization(void);
	~MeshSkeletonization(void);
	virtual void skeletonize(WatertightMesh_ mesh);

	/* ���������ٶȺ�������ƽ���
	 * ��ֵԽ�󣬹���Խ������������ģ��������ٶȻή��
	 * Ĭ��Ϊ0.1 */
	void set_quality_speed_tradeoff(double val);
private:
	/* ��CgalSkeletonת��Skeleton */
	Skeleton_ makeSkeleton( CGALSkeleton& cgalSkeleton );
	double m_quality_speed_tradeoff;
};
S_PTR(MeshSkeletonization);