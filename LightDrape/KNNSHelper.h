#pragma once
#include "nanoflann.hpp"
#include "Vec3d.h"
struct PointCloud
{
	PointCloud(std::vector<Vec3d>& ps);
	PointCloud();
	std::vector<Vec3d>  pts;

	// Must return the number of data points
	size_t kdtree_get_point_count() const;

	// Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
	inline double kdtree_distance(const double *p1, const size_t idx_p2, size_t /*size*/) const
	{
		const double d0 = p1[0] - pts[idx_p2].values_[0];
		const double d1 = p1[1] - pts[idx_p2].values_[1];
		const double d2 = p1[2] - pts[idx_p2].values_[2];
		return d0*d0+d1*d1+d2*d2;
	}

	// Returns the dim'th component of the idx'th point in the class:
	// Since this is inlined and the "dim" argument is typically an immediate value, the
	//  "if/else's" are actually solved at compile time.
	inline double kdtree_get_pt(const size_t idx, int dim) const
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
		Result(size_t i, double d):index(i),distance(d){}
		Result(){}
		size_t index;
		double distance;
	};
	typedef nanoflann::KDTreeSingleIndexAdaptor< nanoflann::L2_Simple_Adaptor<double, PointCloud >, PointCloud, 3 /* dim */> KDTree;

	KNNSHelper(std::vector<Vec3d>& pointList);

	~KNNSHelper();

	/** Return the index of nearest neighbor point and the distance from point q to the nnp 
		return: true if find one nearest.
	**/
	bool singleNeighborSearch(Vec3d q, Result& ret);

	/** Do the k neighbor search 
		return: true if ret.size() == K
	**/
	bool kNeighborSearch(Vec3d& q, int K, std::vector<Result>& ret);

private:	
	KDTree* mKDTree;	
	PointCloud mPointCloud;
};