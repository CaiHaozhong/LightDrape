#include <fstream>
#include "SkeletonContainer.h"
#include "SkeletonDef.h"
#include <iostream>
#include "MeshDef.h"
#include "KNNSHelper.h"
class SkeletonUtility
{
public:

	SkeletonUtility();
	~SkeletonUtility();

	void write(const Skeleton& skeleton, const char* file);

	void read(Skeleton& skeleton, const char* file);

	void readIntoContainer(const char* file);

	void skeletonMatch(Skeleton& human, Skeleton& cloth);

	void recomputeCorrepspondence(Skeleton& skeleton, Mesh& mesh, int numberOfCorrepondencePoint);
private:

	float dis(Skeleton& skeleton, const size_t from, const size_t to);

	SkeletonNode::Point interpolate(float from, float to, SkeletonNode::Point& fromPoint, SkeletonNode::Point& toPoint, float target);

	SkeletonNode::Point pointSub(const SkeletonNode::Point& a, const SkeletonNode::Point& b);

	void skeletonMatch(Skeleton& human, Skeleton& cloth, std::vector<size_t>& humanHandVertices, std::vector<size_t>& clothHandVertices);
};
