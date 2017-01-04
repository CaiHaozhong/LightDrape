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
/* 对Mesh进行骨骼提取，将提取到的骨骼传入该Mesh对象，将“顶点->骨骼节点”的
 * 属性传入该Mesh对象
 * 要求Mesh为WatertightMesh
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
private:
	/* 将CgalSkeleton转成Skeleton */
	Skeleton_ makeSkeleton( CGALSkeleton& cgalSkeleton );
};
S_PTR(MeshSkeletonization);