#pragma once
#include "Meshsegmenter.h"
#include "Common.h"
#include "ClothSegment.h"
#include "Region.h"
class ClothSegmenter :
	public MeshSegmenter
{
public:
	ClothSegmenter(WatertightMesh_ mesh) : MeshSegmenter(mesh){
		/* C++构造函数中调用虚函数时是不会调用子类函数的，所以分开调用init.(Java是可以的） */
		initClothSegmenter(mesh);
	}
	ClothSegmenter(void){}

	void init(WatertightMesh_ mesh){		
		MeshSegmenter::init(mesh);
		initClothSegmenter(mesh);
	}
	
	~ClothSegmenter(void);
private:
	Region_ mTorso;
	Region_ mLeftSleeve;
	Region_ mRightSleeve;
	size_t mTorsoSkeNode, mLeftSleeveSkeNode, mRightSleeveSkeNode;
	WatertightMesh_ mMesh;
protected:
	/* Override */
	void onDifferentLevelSet(size_t seq, LevelSet_ levelSet){		
		if(seq == 1){
			if(levelSet->getCount() != 1){
				PRINTLN("Cloth Segment Error: incorrect categories of torso");
				return ;
			}
			addToRegion(mTorso, levelSet->getCircle(0));
		}
		else if(seq == 2){
			if(levelSet->getCount() != 3){
				PRINTLN("Cloth Segment Error: incorrect categories of sleeves");
				return ;
			}
			if(mTorsoSkeNode == -1){
				typedef std::pair<LevelCircle_, double> CircleCenterPair;				
				WatertightMesh_ mesh = MeshSegmenter::getMesh();
				LevelCircle_ lc0 = levelSet->getCircle(0);
				LevelCircle_ lc1 = levelSet->getCircle(1);
				LevelCircle_ lc2 = levelSet->getCircle(2);				
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
					LevelCircle_ lc = levelSet->getCircle(count);
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
	void onFinishSegment(){
		Segment_ seg = mMesh->getSegment();
		std::vector< std::pair<size_t, Region_> >& regions = seg->getRegionsRaw();
		for(std::vector< std::pair<size_t, Region_> >::iterator it = regions.begin();
			it != regions.end(); it++){
				it->second->expand();
		}
	}

	/* Override */
	Segment_ createSegment(){
		ClothSegment_ mClothSegment = smartNew(ClothSegment);
		mTorso = smartNew(Region);
		mLeftSleeve = smartNew(Region);
		mRightSleeve = smartNew(Region);
		mClothSegment->addRegion(ClothSegment::CLOTH_TORSO, mTorso);
		mClothSegment->addRegion(ClothSegment::CLOTH_LEFT_SLEEVE, mLeftSleeve);
		mClothSegment->addRegion(ClothSegment::CLOTH_RIGHT_SLEEVE, mRightSleeve);
		return mClothSegment;
	}

private:
	void initClothSegmenter(WatertightMesh_ mesh){
		mTorsoSkeNode = mLeftSleeveSkeNode = mRightSleeveSkeNode = -1;
		mMesh = mesh;
	}	

};

