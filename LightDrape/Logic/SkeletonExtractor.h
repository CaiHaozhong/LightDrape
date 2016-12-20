#ifndef SKELETON_EXTRACTOR
#define SKELETON_EXTRACTOR
#include "Mesh.h"
#include "Skeleton.h"
#include "MyPrecompileHeader.h"
// #include <CGAL/Simple_cartesian.h>
// #include <CGAL/Surface_mesh.h>
// #include <CGAL/boost/graph/graph_traits_Surface_mesh.h>
// #include <CGAL/boost/graph/properties_Surface_mesh.h>
// #include <CGAL/Mean_curvature_flow_skeletonization.h>
#include <string>
/*
 * ��ȡ�������������洢��Mesh��Ķ�����
 */
class SkeletonExtractor{
public:
	SkeletonExtractor();
	~SkeletonExtractor();
	void extract(Mesh& mesh);
	void dumpSkeleton(Skeleton& skeleton, std::string file);
	void dumpMesoSkeleton(std::string file);

private:
	
	Skeleton* makeSkeleton(Skeletonization::Skeleton& cgalSkeleton);

	Skeletonization* mcs;
};

#endif