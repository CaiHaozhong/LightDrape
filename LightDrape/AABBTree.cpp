#include "AABBTree.h"
#include "Mesh.h"

AABBTree::AABBTree(void)
{
	mTree = nullptr;
}


AABBTree::~AABBTree(void)
{
	if(mTree != nullptr)
		delete mTree;
}

void AABBTree::initWithMesh( Mesh_ mesh )
{
	std::list<Triangle>* triangles = new std::list<Triangle>;
	for(auto it = mesh->faces_begin(); it != mesh->faces_end(); it++){
		Point points[3];
		int cur = 0;
		for(auto f_v = mesh->fv_begin(*it); f_v.is_valid(); f_v++){
			const Vec3d& p = mesh->point(*f_v);
			points[cur] = Point(p.values_[0], p.values_[1], p.values_[2]);
			cur += 1;
		}
		triangles->push_back(Triangle(points[0], points[1], points[2]));
	}
	mTree = new Tree(triangles->begin(),triangles->end());
}

bool AABBTree::intersection( LineSegment& seg, Vec3d& ret )
{
	Segment_intersection intersection = mTree->any_intersection(seg);
	if(intersection)
	{
		// gets intersection object
		const Point* p = boost::get<Point>(&(intersection->first));	
		if(p){
			ret = Vec3d(p->x(), p->y(), p->z());
			//std::cout << "intersection object is a point " << *p << std::endl;
			return true;
		}
		return false;
	}
	return false;	
}

bool AABBTree::fastIntersectionTest( LineSegment& seg )
{
	return mTree->do_intersect(seg);
}
