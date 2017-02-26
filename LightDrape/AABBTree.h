#pragma once
#include "Common.h"
#include "Vec3d.h"
#include <CGAL/Simple_cartesian.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_triangle_primitive.h>
typedef CGAL::Simple_cartesian<double> K;
typedef K::FT FT;
typedef K::Ray_3 Ray;
typedef K::Line_3 Line;
typedef K::Point_3 Point;
typedef K::Segment_3 LineSegment;
typedef K::Triangle_3 Triangle;
typedef std::list<Triangle>::iterator Iterator;
typedef CGAL::AABB_triangle_primitive<K, Iterator> Primitive;
typedef CGAL::AABB_traits<K, Primitive> AABB_triangle_traits;
typedef CGAL::AABB_tree<AABB_triangle_traits> Tree;
typedef boost::optional< Tree::Intersection_and_primitive_id<LineSegment>::Type > Segment_intersection;
S_PTR(Tree);
class Mesh;
S_PTR(Mesh);

class AABBTree
{
public:
	AABBTree(void);
	~AABBTree(void);
	void initWithMesh(Mesh_ mesh);
	bool intersection(LineSegment& seg, Vec3d& ret);

	bool fastIntersectionTest(LineSegment& seg);
private:
	Tree* mTree;
};
S_PTR(AABBTree);