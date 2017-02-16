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
	std::vector<Vec3d> cicleDeltas;
	std::vector<double> circleHeights;
	size_t garCircleSize = garmetCircles.size();
	size_t humanCircleSize = humanCircles.size();
	cicleDeltas.reserve(garCircleSize);
	circleHeights.reserve(garCircleSize);
	for(size_t i = 0; i < garCircleSize; i++){
		if(i >= humanCircleSize){
			// 比躯干长的部分保持不动
			break;
		}
		Vec3d garCenter = garmetCircles[i]->getMeanPoint(mGarmentRegion->getMesh());
		Vec3d humanCenter = humanCircles[i]->getMeanPoint(humanRegion->getMesh());
		cicleDeltas.push_back(humanCenter-garCenter);
		circleHeights.push_back(garmetCircles[i]->getHeight());
	}

	VertexAlter_ ret = smartNew(VertexAlter);
	computeTranslation(ret, mGarmentRegion->getMesh(), cicleDeltas, circleHeights);
	return ret;
}

std::string LevelSetSkeletonFitter::getName()
{
	return "LevelSetSkeletonFitter";
}

void LevelSetSkeletonFitter::computeTranslation( VertexAlter_ alter, WatertightMesh_ garMesh, 
												std::vector<Vec3d>& circleDeltas, 
												std::vector<double>& circleHeights )
{
	const std::set<size_t>& regionVertices = mGarmentRegion->getVertices();
	size_t circleCount = circleDeltas.size();
	for(auto it = regionVertices.begin(); it != regionVertices.end(); it++){
		size_t ver = *it;
		double geoDis = garMesh->getGeodesicDis(ver);
		size_t cur = 0;
		while(cur < circleCount && geoDis > circleHeights[cur]){
			cur++;
		}
		if(cur == 0){
			alter->add(ver, circleDeltas[0]);
		}
		else if(cur >= circleCount){
			alter->add(ver, Vec3d(0,0,0)/*circleDeltas[circleCount-1]*/);
		}
		else{
			double disLow = circleHeights[cur-1];
			double disHei = circleHeights[cur];
			double lowPart = (geoDis - disLow)/(disHei - disLow);
			alter->add(ver,
				circleDeltas[cur-1] * (1-lowPart) + circleDeltas[cur] * lowPart);
		}
	}
// 	/* key：网格顶点的下标 
//      * value：pair中Vec3d为累积的位移，size_t为影响该网格顶点的骨骼节点的个数
// 	 */
// 	std::unordered_map<size_t, std::pair<Vec3d, size_t> > vertexDeltaMap;
// 	vertexDeltaMap.reserve(mGarmentRegion->getVerticesSize());
// 	size_t circleCount = delta.size();
// 	for(size_t i = 0; i < circleCount; i++){
// 		Vec3d& d = delta[i];
// 		LevelCircle_& cir = circles[i];
// 		std::vector<LevelNode_>& nodes = cir->levelNodes;
// 		for(auto it = nodes.begin(); it != nodes.end(); it++){
// 			size_t ver = (*it)->getNearestVertex(garMesh);
// 			if(vertexDeltaMap.find(ver) == vertexDeltaMap.end()){
// 				vertexDeltaMap[ver] = std::make_pair(d, 1);
// 			}
// 			else{
// 				auto& val = vertexDeltaMap[ver];
// 				val.first += d;
// 				val.second += 1;
// 			}
// 		}
// 	}
// 	VertexAlter_ ret = smartNew(VertexAlter);
// 	for(auto it = vertexDeltaMap.begin(); it != vertexDeltaMap.end(); it++){
// 		ret->add(it->first, it->second.first / it->second.second); // 多个位移的平均值
// 	}
// 	return ret;
}
