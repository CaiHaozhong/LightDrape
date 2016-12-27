#pragma once
#include "Meshsegmenter.h"
#include "Common.h"
#include "ClothSegment.h"
#include "Region.h"
class ClothSegmenter :
	public MeshSegmenter
{
public:
	ClothSegmenter(void){
		mClothSegment = smartNew(ClothSegment);
		mTorso = smartNew(Region);
		mLeftSleeve = smartNew(Region);
		mRightSleeve = smartNew(Region);
		mClothSegment->addRegion(ClothSegment::CLOTH_TORSO, mTorso);
		mClothSegment->addRegion(ClothSegment::CLOTH_LEFT_SLEEVE, mLeftSleeve);
		mClothSegment->addRegion(ClothSegment::CLOTH_RIGHT_SLEEVE, mRightSleeve);
		Mesh_ mesh = MeshSegmenter::getMesh();
		hasAdded = smartNew(BooleanProperty);
		mesh->registerProperty(hasAdded);
		for(size_t i = 0; i < mesh->n_vertices(); i++){
			hasAdded->set(i, false);
		}
		hasSkeletonNodeAdded.resize(MeshSegmenter::getMesh()->getSkeleton()->nodeCount(), false);
		mTorsoSkeNode = mLeftSleeveSkeNode = mRightSleeveSkeNode = -1;
	}
	~ClothSegmenter(void);
private:
	ClothSegment_ mClothSegment;
	Region_ mTorso;
	Region_ mLeftSleeve;
	Region_ mRightSleeve;
	BooleanProperty_ hasAdded;//TODO：测试一下是否回收了
	std::vector<bool> hasSkeletonNodeAdded;//判断骨骼节点是否已经加入Region了
	size_t mTorsoSkeNode, mLeftSleeveSkeNode, mRightSleeveSkeNode;
protected:
	/* Override */
	void onDifferentLevelSet(size_t seq, LevelSet& levelSet){		
		if(seq == 1){
			if(levelSet.getCount() != 1){
				PRINTLN("Cloth Segment Error: incorrect categories of torso");
				return ;
			}
			addToRegion(mTorso, levelSet.getCircle(0));
		}
		else if(seq == 2){
			if(levelSet.getCount() != 3){
				PRINTLN("Cloth Segment Error: incorrect categories of sleeves");
				return ;
			}
			if(mTorsoSkeNode == -1){
				typedef std::pair<LevelCircle_, double> CircleCenterPair;				
				WatertightMesh_ mesh = MeshSegmenter::getMesh();
				LevelCircle_ lc0 = levelSet.getCircle(0);
				LevelCircle_ lc1 = levelSet.getCircle(1);
				LevelCircle_ lc2 = levelSet.getCircle(2);				
				CircleCenterPair ccp[3] = {
				std::make_pair(lc0, lc0->getCenterX(mesh)),
				std::make_pair(lc1, lc1->getCenterX(mesh)),
				std::make_pair(lc2, lc2->getCenterX(mesh))};
				std::sort(ccp, ccp + 3, [](const CircleCenterPair& a, const CircleCenterPair& b){
					return a.second < b.second;
				});
				mLeftSleeveSkeNode = getCircleSkeletonNode(ccp[0].first);
				mTorsoSkeNode = getCircleSkeletonNode(ccp[1].first);
				mRightSleeveSkeNode = getCircleSkeletonNode(ccp[2].first);
				addToRegion(mLeftSleeve, ccp[0].first);
				addToRegion(mTorso, ccp[1].first);
				addToRegion(mRightSleeve, ccp[2].first);
			}
			else{
				int count = 3;
				Skeleton_ skeleton = MeshSegmenter::getMesh()->getSkeleton();
				while(count--){
					LevelCircle_ lc = levelSet.getCircle(count);
					size_t skeNode = getCircleSkeletonNode(lc);
					if(skeleton->isNodeNeighbor(skeNode, mLeftSleeveSkeNode)){
						mLeftSleeveSkeNode = skeNode;
						addToRegion(mLeftSleeve, lc);
					}
					else if(skeleton->isNodeNeighbor(skeNode, mTorsoSkeNode)){
						mTorsoSkeNode = skeNode;
						addToRegion(mTorso, lc);
					}
					else if(skeleton->isNodeNeighbor(skeNode, mRightSleeveSkeNode)){
						mRightSleeveSkeNode = skeNode;
						addToRegion(mRightSleeve, lc);
					}
				}
			}
		}
	}

	/* Override */
	Segment_ createSegment(){
		return mClothSegment;
	}

private:
	/* 将一个LevelCircle加到一个Region中 */
	void addToRegion(Region_ region, LevelCircle_ levelCircle){
		std::vector<LevelNode_> levelNodes = levelCircle->levelNodes;
		WatertightMesh_ mesh = MeshSegmenter::getMesh();
		for(size_t i = 0; i < levelNodes.size(); i++){
			LevelNode_ n = levelNodes[i];
			size_t v = n->getNearestVertex(mesh);
			if(!hasAdded->get(v)){
				mTorso->addVertex(v);
				hasAdded->set(v, true);
			}
			size_t skenode = mesh->getCorrSkeletonNode(v);
			if(!hasSkeletonNodeAdded[skenode]){
				mTorso->addSkeleton(skenode);
				hasSkeletonNodeAdded[skenode] = true;
			}
		}
	}
	/* 获取一个LevelCircle对应的骨骼节点
	 * 返回：对应这个LevelCircle中最多个节点的骨骼节点 
	 */
	size_t getCircleSkeletonNode(LevelCircle_ levelCircle){
		WatertightMesh_ mesh = MeshSegmenter::getMesh();
		std::map<size_t, size_t> nodeCountMap;
		for(size_t i = 0; i < levelCircle->levelNodes.size(); i++){
			LevelNode_ n = levelCircle->levelNodes[i];
			size_t v = n->getNearestVertex(mesh);
			size_t skeNode = mesh->getCorrSkeletonNode(v);
			if(nodeCountMap.find(skeNode) == nodeCountMap.end()){
				nodeCountMap[skeNode] = 1;
			}
			else{
				nodeCountMap[skeNode] += 1;
			}
		}
		int maxCount = -1;
		size_t ret = -1;
		for(std::map<size_t, size_t>::iterator it = nodeCountMap.begin();
			it != nodeCountMap.end(); it++){
				if(it->second > maxCount){
					maxCount = it->second;
					ret = it->first;
				}
		}
		if(ret == -1){
			PRINTLN("ERROR! In getCircleSkeletonNode. ret == -1");
		}
		return -1;
	}
};

