#pragma once
#include "SkeletonFitter.h"
#include "Vec3d.h"
#include <vector>
#include <tuple>

class SimpleSkeletonFitter :
	public SkeletonFitter
{
public:
	SimpleSkeletonFitter(void);
	SimpleSkeletonFitter(Region_ garmentRegion);
	~SimpleSkeletonFitter(void);
	VertexAlter_ fit(Region_ humanRegion);
	std::string getName();

protected:
	/* Override
 	 * 获取起始骨骼节点 */
	size_t getStartSkeletonNode(Region_ region);

	/* Override
	 * 获取该Region的骨骼 */
	Skeleton_ getSkeleton(Region_ region);

	/* Override
	 * 获取处于该region中的骨骼节点的集合 */
	std::set<size_t>& getSkeletonNodesInRegion(Region_ region);

};
S_PTR(SimpleSkeletonFitter);
