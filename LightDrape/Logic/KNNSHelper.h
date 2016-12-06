#pragma once
#include "nanoflann.hpp"
#include <OpenMesh/Core/Geometry/VectorT.hh>
// #include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
// #include <CGAL/Search_traits_3.h>
// #include <CGAL/Search_traits_adapter.h>
// #include <CGAL/point_generators_3.h>
// #include <CGAL/Orthogonal_k_neighbor_search.h>
// #include <boost/iterator/counting_iterator.hpp>
// #include <utility>
// typedef CGAL::Exact_predicates_inexact_constructions_kernel EpicKernel;
// typedef EpicKernel::Point_3 Point_3;
// typedef std::size_t Point;
//definition of a non-mutable lvalue property map,
//with the get function as a friend function to give it
//access to the private member
// class My_point_property_map{
// 	const std::vector<Point_3>& points;
// public:
// 	typedef Point_3 value_type;
// 	typedef const value_type& reference;
// 	typedef Point key_type;
// 	typedef boost::lvalue_property_map_tag category;  
// 	My_point_property_map(const std::vector<Point_3>& pts):points(pts){}
// 	reference operator[](key_type k) const {return points[k];}
// 	friend reference get(const My_point_property_map& ppmap,key_type i) 
// 	{return ppmap[i];}
// };
// typedef CGAL::Random_points_in_cube_3<Point_3>                               Random_points_iterator;
// typedef CGAL::Search_traits_3<EpicKernel>                                        Traits_base;
// typedef CGAL::Search_traits_adapter<Point,My_point_property_map,Traits_base> Traits;
// typedef CGAL::Orthogonal_k_neighbor_search<Traits>                      K_neighbor_search;
// typedef K_neighbor_search::Tree                                         Tree;
// typedef Tree::Splitter                                                  Splitter;
// typedef K_neighbor_search::Distance                                     Distance;
/**usage:
/**
#include "KNNSHelper.h"
typedef CGAL::Random_points_in_cube_3<Point_3>                               Random_points_iterator;
int main()
{
	Random_points_iterator rpit( 1.0);
	std::vector<Point_3> points;
	for (int i = 0; i < 100000; i++)
	{
		points.push_back(*(rpit++));
	}
	KNNSHelper knnsHelper(points);
	//std::pair<int, double> ret = knnsHelper.singleNeighborSearch(Point_3(0,0,0));
	knnsHelper.kNeighborSearch(Point_3(0,0,0),10);
	for (KNNSHelper::KNNSHelperIterator it = knnsHelper.begin(); it != knnsHelper.end(); it++)
	{
		std::cout << knnsHelper.getIndex(it) << ' ' << knnsHelper.getDistance(it) << std::endl;
	}	
	getchar();
}
**/
struct PointCloud
{
	PointCloud(std::vector<OpenMesh::Vec3f>& ps){
		pts = ps;
	}
	PointCloud(){}
	std::vector<OpenMesh::Vec3f>  pts;

	// Must return the number of data points
	inline size_t kdtree_get_point_count() const { 
		size_t ret = pts.size();
		return ret; 
	}

	// Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
	inline float kdtree_distance(const float *p1, const size_t idx_p2, size_t /*size*/) const
	{
		const float d0 = p1[0] - pts[idx_p2].values_[0];
		const float d1 = p1[1] - pts[idx_p2].values_[1];
		const float d2 = p1[2] - pts[idx_p2].values_[2];
		return d0*d0+d1*d1+d2*d2;
	}

	// Returns the dim'th component of the idx'th point in the class:
	// Since this is inlined and the "dim" argument is typically an immediate value, the
	//  "if/else's" are actually solved at compile time.
	inline float kdtree_get_pt(const size_t idx, int dim) const
	{
		if (dim==0) return pts[idx].values_[0];
		else if (dim==1) return pts[idx].values_[1];
		else return pts[idx].values_[2];
	}

	// Optional bounding-box computation: return false to default to a standard bbox computation loop.
	//   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
	//   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
	template <class BBOX>
	bool kdtree_get_bbox(BBOX& /* bb */) const { return false; }

};
class KNNSHelper
{
public:	
	struct Result{
		Result(size_t i, float d):index(i),distance(d){}
		Result(){}
		size_t index;
		float distance;
	};
	typedef nanoflann::KDTreeSingleIndexAdaptor< nanoflann::L2_Simple_Adaptor<float, PointCloud >, PointCloud, 3 /* dim */> KDTree;

	KNNSHelper(std::vector<OpenMesh::Vec3f>& pointList);

	~KNNSHelper();

	/** Return the index of nearest neighbor point and the distance from point q to the nnp 
		return: true if find one nearest.
	**/
	bool singleNeighborSearch(OpenMesh::Vec3f q, Result& ret);

	/** Do the k neighbor search 
		return: true if ret.size() == K
	**/
	bool kNeighborSearch(OpenMesh::Vec3f& q, int K, std::vector<Result>& ret);

private:	
	KDTree* mKDTree;
	PointCloud mPointCloud;
};