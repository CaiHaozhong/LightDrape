#include "LevelSetSkeletonFitter.h"
#include "LevelSet.h"
#include "Vec3d.h"
#include <vector>
#include <unordered_map>
#include "VertexAlter.h"

LevelSetSkeletonFitter::LevelSetSkeletonFitter(void)
{
}

LevelSetSkeletonFitter::LevelSetSkeletonFitter( Region_ garmentRegion ):RegionFitter(garmentRegion){}


LevelSetSkeletonFitter::~LevelSetSkeletonFitter(void)
{
}

VertexAlter_ LevelSetSkeletonFitter::fit( Region_ humanRegion )
{
	mHumanRegion = humanRegion;
	std::vector<LevelCircle_>& humanCircles = humanRegion->getCircles();
	std::vector<LevelCircle_>& garmetCircles = mGarmentRegion->getCircles();
	std::vector<Vec3d> delta;
	size_t garCircleSize = garmetCircles.size();
	size_t humanCircleSize = humanCircles.size();
	delta.reserve(garCircleSize);
	for(size_t i = 0; i < garCircleSize; i++){
		if(i >= humanCircleSize){
			// 比躯干长的部分保持不动
			break;
		}
		Vec3d garCenter = garmetCircles[i]->getMeanPoint(mGarmentRegion->getMesh());
		Vec3d humanCenter = humanCircles[i]->getMeanPoint(humanRegion->getMesh());
		delta.push_back(humanCenter-garCenter);
	}
	return computeVertexAlter(mGarmentRegion->getMesh(), delta, garmetCircles);
}

std::string LevelSetSkeletonFitter::getName()
{
	return "LevelSetSkeletonFitter";
}

VertexAlter_ LevelSetSkeletonFitter::computeVertexAlter( Mesh_ garMesh, std::vector<Vec3d>& delta, 
														std::vector<LevelCircle_>& circles )
{
	/* key：网格顶点的下标 
     * value：pair中Vec3d为累积的位移，size_t为影响该网格顶点的骨骼节点的个数
	 */
	std::unordered_map<size_t, std::pair<Vec3d, size_t> > vertexDeltaMap;
	vertexDeltaMap.reserve(mGarmentRegion->getVerticesSize());
	size_t circleCount = delta.size();
	for(size_t i = 0; i < circleCount; i++){
		Vec3d& d = delta[i];
		LevelCircle_& cir = circles[i];
		std::vector<LevelNode_>& nodes = cir->levelNodes;
		for(auto it = nodes.begin(); it != nodes.end(); it++){
			size_t ver = (*it)->getNearestVertex(garMesh);
			if(vertexDeltaMap.find(ver) == vertexDeltaMap.end()){
				vertexDeltaMap[ver] = std::make_pair(d, 1);
			}
			else{
				auto& val = vertexDeltaMap[ver];
				val.first += d;
				val.second += 1;
			}
		}
	}
	VertexAlter_ ret = smartNew(VertexAlter);
	for(auto it = vertexDeltaMap.begin(); it != vertexDeltaMap.end(); it++){
		ret->add(it->first, it->second.first / it->second.second); // 多个位移的平均值
	}
	return ret;
}
