#include "KNNSHelper.h"

KNNSHelper::KNNSHelper( std::vector<Point_3>& points )
{
	My_point_property_map ppmap(points);
	// Insert number_of_data_points in the tree
	this->mKdTree = new Tree(
		boost::counting_iterator<std::size_t>(0),
		boost::counting_iterator<std::size_t>(points.size()),
		Splitter(),
		Traits(ppmap)
		);
	this->tr_dist = new Distance(ppmap);
	this->mSearch = nullptr;
}

KNNSHelper::~KNNSHelper()
{
	if(mKdTree != nullptr)
		delete mKdTree;
	if(tr_dist != nullptr)
		delete tr_dist;
	if(mSearch != nullptr)
		delete mSearch;
}

std::pair<int, double> KNNSHelper::singleNeighborSearch( Point_3& q )
{
	kNeighborSearch(q,1);
	return std::make_pair(getIndex(mIteratorBegin), getDistance(mIteratorBegin));
}

void KNNSHelper::kNeighborSearch( Point_3& q, int K )
{
	if(mSearch != nullptr)
		delete mSearch;
	mSearch = new K_neighbor_search(*mKdTree, q, K,0,true,*tr_dist);	
	mIteratorBegin = mSearch->begin();
	mIteratorEnd = mSearch->end();	
}

int KNNSHelper::getIndex( KNNSHelperIterator it )
{
	return it->first;
}

double KNNSHelper::getDistance( KNNSHelperIterator it )
{
	return tr_dist->inverse_of_transformed_distance(it->second);
}

KNNSHelper::KNNSHelperIterator KNNSHelper::begin()
{
	return mIteratorBegin;
}

KNNSHelper::KNNSHelperIterator KNNSHelper::end()
{
	return mIteratorEnd;
}

