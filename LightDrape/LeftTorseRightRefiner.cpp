#include "LeftTorseRightRefiner.h"
#include "Region.h"
#include "LevelSet.h"
#include "MeshSegmenter.h"

LeftTorseRightRefiner::LeftTorseRightRefiner(void)
{
	mLeft = mRight = mTorse = nullptr;
}

LeftTorseRightRefiner::LeftTorseRightRefiner( MeshSegmenter_ meshSegmenter, Region_ left, Region_ torse, Region_ right )
{
	mMeshSegmenter = meshSegmenter;
	mLeft = left;
	mTorse = torse;
	mRight = right;
}


LeftTorseRightRefiner::~LeftTorseRightRefiner(void)
{
}

void LeftTorseRightRefiner::refine()
{
	if(mLeft == nullptr || mRight == nullptr || mTorse == nullptr)
		return;
	if(mLeft->getCircles().size() <= 0 || mRight->getCircles().size() <= 0){
		PRINTLN("In LeftTorseRightRefiner, Region empty!");
		return;
	}
	LevelCircle_ leftTopCircle = mLeft->getCircles()[0];
	LevelCircle_ rightTopCircle = mRight->getCircles()[0];
	LevelSet_ topLevelSet = leftTopCircle->getParent();
	Mesh_ mMesh = mMeshSegmenter->getMesh();
	if(topLevelSet == nullptr) return ;
	std::unordered_set<size_t> leftSet, rightSet, torseSet;
	addCircleToHashSet(leftSet, leftTopCircle);
	addCircleToHashSet(rightSet, rightTopCircle);
	for(size_t i = 0; i < topLevelSet->getCount(); i++){
		LevelCircle_ lc = topLevelSet->getCircle(i);
		if(lc != leftTopCircle && lc != rightTopCircle){
			addCircleToHashSet(torseSet, lc);
			break;
		}
	}
	/* 位于手臂的首端的LevelSet的下标 */
	size_t topLevelSetIndex = mMeshSegmenter->getLevelSetIndex(topLevelSet);
	size_t curLevelSetIndex = topLevelSetIndex;	
	while(curLevelSetIndex--){
		bool allNodesInTorse = true;
		LevelSet_ curLS = mMeshSegmenter->getLevelSet(curLevelSetIndex);
		std::vector<LevelNode_> nodes;
		getNodesFromLevelSet(curLS, nodes);	
		std::vector<size_t> versInLeftRegion, versInRightRegion;
		versInLeftRegion.reserve(nodes.size()/2);
		versInRightRegion.reserve(nodes.size()/2);
		for(auto it = nodes.begin(); it != nodes.end(); it++){
			size_t v = (*it)->getNearestVertex(mMesh);
			size_t start_v = (*it)->start_vertex;
			size_t to_v = (*it)->getToVertexIndex(mMesh);
			size_t ends[2] = {start_v, to_v};
/*			int locs[2];*/
			for(int i = 0; i < 2; i++){
				size_t ver = ends[i];
				int location = decideLocation(leftSet, rightSet, torseSet, ver);
/*				locs[i] = location;*/
				switch (location)
				{
				case IN_LEFT:
					allNodesInTorse = false;
//					mLeft->addVertex(ver);
					versInLeftRegion.push_back(ver);
					leftSet.insert(ver);
					break;
				case IN_RIGHT:
					allNodesInTorse = false;
//					mRight->addVertex(ver);
					versInRightRegion.push_back(ver);
					rightSet.insert(ver);
					break;
				case IN_TORSE:
					torseSet.insert(ver);
					break;
				default:
					std::cout << "isolate: " << curLevelSetIndex << " ";
					break;
				}
			}
// 			if(locs[0] == locs[1]){
// 				if(locs[0] == IN_LEFT){
// 					curLevelSetInLeft->addNode(*it);
// 				}
// 				else if(locs[0] == IN_RIGHT){
// 					curLevelSetInRight->addNode(*it);
// 				}
// 			}
// 			else{				
// 				PRINT("End vertices in different Region: ");
// 				std::cout << locs[0] << " " << locs[1] << "\n";
// 			}
		}
		if(allNodesInTorse)//当一个LevelSet所有的节点都位于Torse，则退出
			break;
		if(versInLeftRegion.size() > 8){
			mLeft->getRegionSkeleton()->push_front(versInLeftRegion, Region::computeCenter(versInLeftRegion, mMesh), curLS->getHeight());
			for(auto it = versInLeftRegion.begin(); it != versInLeftRegion.end(); it++){
				mLeft->addVertex(*it);
			}
		}
		if(versInRightRegion.size() > 8){
			mRight->getRegionSkeleton()->push_front(versInRightRegion, Region::computeCenter(versInRightRegion, mMesh), curLS->getHeight());
			for(auto it = versInRightRegion.begin(); it != versInRightRegion.end(); it++){
				mRight->addVertex(*it);
			}
		}
	}
	regionSub(mTorse, mLeft);
	regionSub(mTorse, mRight);
}

int LeftTorseRightRefiner::decideLocation( std::unordered_set<size_t>& leftSet, 
										  std::unordered_set<size_t>& rightSet, 
										  std::unordered_set<size_t>& torseSet, 
										  size_t v )
{
	if(leftSet.find(v) != leftSet.end())
		return IN_LEFT;
	else if(rightSet.find(v) != rightSet.end()) 
		return IN_RIGHT;
	else if(torseSet.find(v) != torseSet.end()) {
		return IN_TORSE;				
	}
	else{
		int ret = -1;
		for(auto vv_it = mMeshSegmenter->getMesh()->vv_begin(Mesh::VertexHandle(v)); 
			vv_it.is_valid(); vv_it++){
				size_t v_nei = vv_it->idx();
				if(leftSet.find(v_nei) != leftSet.end()) 
					ret = IN_LEFT;
				else if(rightSet.find(v_nei) != rightSet.end()) 
					ret = IN_RIGHT;
				else if(torseSet.find(v_nei) != torseSet.end()) {
					ret = IN_TORSE;
					break;
				}
		}
		return ret;
	}	
}

void LeftTorseRightRefiner::addCircleToHashSet( std::unordered_set<size_t>& set, LevelCircle_ circle )
{
	std::vector<LevelNode_>& nodes = circle->levelNodes;
	size_t count = nodes.size();
	for(size_t i = 0; i < count; i++){
		set.insert(nodes[i]->start_vertex);
		set.insert(nodes[i]->getToVertexIndex(mMeshSegmenter->getMesh()));
	}
}

void LeftTorseRightRefiner::insertCircleToRegion(Region_ region, LevelCircle_ circle)
{
	auto& vec = region->getCircles();
	vec.insert(vec.begin(), circle);
}

void LeftTorseRightRefiner::regionSub( Region_ a, Region_ b )
{
	std::set<size_t>& verOfA = a->getVertices();
	std::set<size_t>& verOfB = b->getVertices();
	for(auto it = verOfB.begin(); it != verOfB.end(); it++){
		verOfA.erase(*it);
	}
}

void LeftTorseRightRefiner::getVertexFromLevelSet( LevelSet_ ls, std::vector<size_t>& ret )
{
	Mesh_ mesh = mMeshSegmenter->getMesh();
	for(size_t i = 0; i < ls->getCount(); i++){
		LevelCircle_ lc = ls->getCircle(i);
		std::vector<LevelNode_>& nodes = lc->levelNodes;
		for(auto it = nodes.begin(); it != nodes.end(); it++){
			ret.push_back((*it)->start_vertex);
			ret.push_back((*it)->getToVertexIndex(mesh));
		}
	}
}

void LeftTorseRightRefiner::getNodesFromLevelSet( LevelSet_ ls, std::vector<LevelNode_>& ret )
{
	size_t circleCount = ls->getCount();
	for(size_t i = 0; i < circleCount; i++){
		LevelCircle_ lc = ls->getCircle(i);
		auto& nodes = lc->levelNodes;
		for(auto it = nodes.begin(); it != nodes.end(); it++){
			ret.push_back(*it);
		}
	}
}

void LeftTorseRightRefiner::addVerticesToHashSet( std::unordered_set<size_t>& ret, std::vector<size_t>& vertices )
{
	for(auto it = vertices.begin(); it != vertices.end(); it++){
		ret.insert(*it);
	}
}