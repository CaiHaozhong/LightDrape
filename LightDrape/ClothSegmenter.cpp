#include "ClothSegmenter.h"



ClothSegmenter::~ClothSegmenter(void)
{
}

ClothSegmenter::ClothSegmenter( WatertightMesh_ mesh ) : MeshSegmenter(mesh)
{
	/* C++构造函数中调用虚函数时是不会调用子类函数的，所以分开调用init.(Java是可以的） */
	initClothSegmenter(mesh);
}

ClothSegmenter::ClothSegmenter( void )
{

}

void ClothSegmenter::init( WatertightMesh_ mesh )
{
	MeshSegmenter::init(mesh);
	initClothSegmenter(mesh);
}

void ClothSegmenter::onDifferentLevelSet( size_t seq, LevelSet_ levelSet )
{
	if(seq == 1 || seq == 3){
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
			LevelCircle_ lc0 = levelSet->getCircle(0);
			LevelCircle_ lc1 = levelSet->getCircle(1);
			LevelCircle_ lc2 = levelSet->getCircle(2);				
			CircleCenterPair ccp[3] = {
				std::make_pair(lc0, lc0->getCenterX(mMesh)),
				std::make_pair(lc1, lc1->getCenterX(mMesh)),
				std::make_pair(lc2, lc2->getCenterX(mMesh))};
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
			for(size_t i = 0; i < 3; i++){
				LevelCircle_ lc = levelSet->getCircle(i);
				size_t skeNode = getCircleSkeletonNode(lc);
				size_t disLH = mMeshSkeleton->intervalNodeCount(skeNode, mLeftSleeveSkeNode);
				size_t disTS = mMeshSkeleton->intervalNodeCount(skeNode, mTorsoSkeNode);
				size_t disRH = mMeshSkeleton->intervalNodeCount(skeNode, mRightSleeveSkeNode);
				if(disLH <= disTS && disLH <= disRH){
					addToRegion(mLeftSleeve, lc);
					mLeftSleeveSkeNode = skeNode;					
				}
				else if(disTS <= disLH && disTS <= disRH){
					addToRegion(mTorso, lc);
					mTorsoSkeNode = skeNode;
				}
				else if(disRH <= disTS && disRH <= disLH){
					addToRegion(mRightSleeve, lc);
					mRightSleeveSkeNode = skeNode;
				}
			}
		}
	}
}

void ClothSegmenter::onFinishCoarseSegment()
{
	Segment_ seg = mMesh->getSegment();
	std::vector< std::pair<int, Region_> >& regions = seg->getRegionsRaw();
	for(std::vector< std::pair<int, Region_> >::iterator it = regions.begin();
		it != regions.end(); it++){
			it->second->expand();
	}
}

Segment_ ClothSegmenter::createSegment()
{
	ClothSegment_ mClothSegment = smartNew(ClothSegment);
	mTorso = std::make_shared<Region>(mMesh);
	mLeftSleeve = std::make_shared<Region>(mMesh);
	mRightSleeve = std::make_shared<Region>(mMesh);
	mClothSegment->addRegion(ClothSegment::CLOTH_TORSO, mTorso);
	mClothSegment->addRegion(ClothSegment::CLOTH_LEFT_SLEEVE, mLeftSleeve);
	mClothSegment->addRegion(ClothSegment::CLOTH_RIGHT_SLEEVE, mRightSleeve);				
	return mClothSegment;
}

void ClothSegmenter::initClothSegmenter( WatertightMesh_ mesh )
{
	mTorsoSkeNode = mLeftSleeveSkeNode = mRightSleeveSkeNode = -1;
	mMesh = mesh;
	mMeshSkeleton = mMesh->getSkeleton();
}
