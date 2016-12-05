#pragma once
#include "Mesh.h"
#include "Skeleton.h"
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/graph_traits_Surface_mesh.h>
#include <CGAL/boost/graph/properties_Surface_mesh.h>
#include <CGAL/Mean_curvature_flow_skeletonization.h>

typedef CGAL::Simple_cartesian<double>                        Kernel;
typedef CGAL::Surface_mesh<Kernel::Point_3>                   Triangle_mesh;
typedef CGAL::Mean_curvature_flow_skeletonization<Triangle_mesh> Skeletonization;
/*
 * 提取骨骼，将骨骼存储在Mesh类的对象中
 */
class SkeletonExtractor{
public:

	static void extract(Mesh& mesh);

private:

	static Skeleton* makeSkeleton(Skeletonization::Skeleton& cgalSkeleton);
};

