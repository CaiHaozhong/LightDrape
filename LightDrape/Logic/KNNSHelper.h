#pragma once
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Search_traits_3.h>
#include <CGAL/Search_traits_adapter.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <boost/iterator/counting_iterator.hpp>
#include <utility>
typedef CGAL::Exact_predicates_inexact_constructions_kernel EpicKernel;
typedef EpicKernel::Point_3 Point_3;
typedef std::size_t Point;
//definition of a non-mutable lvalue property map,
//with the get function as a friend function to give it
//access to the private member
class My_point_property_map{
	const std::vector<Point_3>& points;
public:
	typedef Point_3 value_type;
	typedef const value_type& reference;
	typedef Point key_type;
	typedef boost::lvalue_property_map_tag category;  
	My_point_property_map(const std::vector<Point_3>& pts):points(pts){}
	reference operator[](key_type k) const {return points[k];}
	friend reference get(const My_point_property_map& ppmap,key_type i) 
	{return ppmap[i];}
};
typedef CGAL::Random_points_in_cube_3<Point_3>                               Random_points_iterator;
typedef CGAL::Search_traits_3<EpicKernel>                                        Traits_base;
typedef CGAL::Search_traits_adapter<Point,My_point_property_map,Traits_base> Traits;
typedef CGAL::Orthogonal_k_neighbor_search<Traits>                      K_neighbor_search;
typedef K_neighbor_search::Tree                                         Tree;
typedef Tree::Splitter                                                  Splitter;
typedef K_neighbor_search::Distance                                     Distance;
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
class KNNSHelper
{
public:
	typedef K_neighbor_search::iterator KNNSHelperIterator;

	KNNSHelper(std::vector<Point_3>& pointList);

	~KNNSHelper();

	/** Return the index of nearest neighbor point and the distance from point q to the nnp **/
	std::pair<int, double> singleNeighborSearch(Point_3& q);

	/** Do the k neighbor search **/
	void kNeighborSearch(Point_3& q, int K);

	/** Get the point index **/
	int getIndex(KNNSHelperIterator it);

	/** Get the distance from the query point to the nearest point**/
	double getDistance(KNNSHelperIterator it);

	KNNSHelperIterator begin();

	KNNSHelperIterator end();
private:	
	Tree* mKdTree;
	Distance* tr_dist;	
	K_neighbor_search* mSearch;
	KNNSHelperIterator mIteratorBegin;
	KNNSHelperIterator mIteratorEnd;
};