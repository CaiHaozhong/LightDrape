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
	std::vector<Triangle>* triangles = new std::vector<Triangle>;
	for(auto it = mesh->faces_begin(); it != mesh->faces_end(); it++){

// 		/* 求面的法向量 */
// 		Vec3d faceVers[3];
// 		int _v = 0;
// 		for(auto f_v = mesh->fv_begin(*it); f_v.is_valid(); f_v++){
// 				faceVers[_v++] = mesh->point(*f_v);
// 		}
// 		Vec3d faceNormal = (faceVers[0] - faceVers[1]) % (faceVers[1] - faceVers[2]);
// 		Vec3d n = faceNormal.normalize_cond() * 0.05;

		Point points[3];
		int cur = 0;
		for(auto f_v = mesh->fv_begin(*it); f_v.is_valid(); f_v++){
			const Vec3d& p = mesh->point(*f_v);
			Vec3d n = mesh->normal(*f_v);
			n = n.normalize_cond() * 0.05;
			points[cur] = Point((p+n).values_[0], (p+n).values_[1], (p+n).values_[2]);
			cur += 1;
		}
		triangles->push_back(Triangle(points[0], points[1], points[2]));
	}
	mTriangleBegin = triangles->begin();
	mTree = new Tree(triangles->begin(),triangles->end());
}

bool AABBTree::intersection( LineSegment& seg, Vec3d& ret, size_t& intersectTriangleIndex )
{
	Segment_intersection intersection = mTree->any_intersection(seg);
	if(intersection)
	{
		// gets intersection object
		const Point* p = boost::get<Point>(&(intersection->first));	
		Iterator id = intersection->second;
		intersectTriangleIndex = id - mTriangleBegin;
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
