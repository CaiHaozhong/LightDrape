#include "KNNSHelper.h"

KNNSHelper::KNNSHelper( std::vector<Vec3d>& points )
{
	mPointCloud = PointCloud(points);
	mKDTree = new KDTree(3, mPointCloud, nanoflann::KDTreeSingleIndexAdaptorParams(10 /* max leaf */) );
	mKDTree->buildIndex();
}

KNNSHelper::~KNNSHelper(){
	delete mKDTree;	
}

bool KNNSHelper::singleNeighborSearch(Vec3d q, Result& ret)
{	
	std::vector<Result> rets;
	bool r = kNeighborSearch(q, 1, rets);
	if(r)
		ret = rets.at(0);
	return r;
}

bool KNNSHelper::kNeighborSearch(Vec3d& q, int K, std::vector<Result>& ret)
{
	nanoflann::KNNResultSet<double> resultSet(K);
	size_t* indices = new size_t[K];
	double* dists = new double[K];
	resultSet.init(indices, dists);
	bool r = mKDTree->findNeighbors(resultSet, q.values_, nanoflann::SearchParams(10));
	size_t retSize = resultSet.size();
	for(size_t i = 0; i < retSize; i++){
		ret.push_back(Result(indices[i], dists[i]));
	}
	delete [] indices;
	delete [] dists;
	return r;
}

PointCloud::PointCloud( std::vector<Vec3d>& ps )
{
	pts = ps;
}

PointCloud::PointCloud()
{

}

size_t PointCloud::kdtree_get_point_count() const
{
	size_t ret = pts.size();
	return ret;
}
