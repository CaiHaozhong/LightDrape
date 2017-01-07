#include "HumanSegmenter.h"



HumanSegmenter::~HumanSegmenter(void)
{
}

HumanSegmenter::HumanSegmenter( WatertightMesh_ mesh ) : MeshSegmenter(mesh)
{
	/* C++构造函数中调用虚函数时是不会调用子类函数的，所以分开调用init.(Java是可以的） */
	initHumanSegmenter(mesh);
}

HumanSegmenter::HumanSegmenter( void )
{

}

void HumanSegmenter::init( WatertightMesh_ mesh )
{
	MeshSegmenter::init(mesh);
	initHumanSegmenter(mesh);
}

void HumanSegmenter::onDifferentLevelSet( size_t seq, LevelSet_ levelSet )
{
	if(seq == 1){
		handleHead(levelSet);
	}
	else if(seq == 2){
		handleTorso(levelSet);
	}
	else if(seq == 3){
		handleLegs(levelSet);
	}
}

void HumanSegmenter::onFinishCoarseSegment()
{
	Segment_ seg = mMesh->getSegment();
	std::vector< std::pair<int, Region_> >& regions = seg->getRegionsRaw();
	for(std::vector< std::pair<int, Region_> >::iterator it = regions.begin();
		it != regions.end(); it++){
			it->second->expand();
	}
}

Segment_ HumanSegmenter::createSegment()
{
	Segment_ seg = smartNew(Segment);
	mHead = std::make_shared<Region>(mMeshSkeleton);
	mTorso = std::make_shared<Region>(mMeshSkeleton);
	mLeftHand = std::make_shared<Region>(mMeshSkeleton);
	mRightHand = std::make_shared<Region>(mMeshSkeleton);
	mLeftLeg = std::make_shared<Region>(mMeshSkeleton);
	mRightLeg = std::make_shared<Region>(mMeshSkeleton);
	seg->addRegionRaw(Segment::BODY_HEAD, mHead);
	seg->addRegionRaw(Segment::BODY_TORSE, mTorso);
	seg->addRegionRaw(Segment::BODY_LEFT_HAND, mLeftHand);
	seg->addRegionRaw(Segment::BODY_RIGHT_HAND, mRightHand);
	seg->addRegionRaw(Segment::BODY_LEFT_LEG, mLeftLeg);
	seg->addRegionRaw(Segment::BODY_RIGHT_LEG, mRightLeg);
	return seg;
}

void HumanSegmenter::initHumanSegmenter( WatertightMesh_ mesh )
{
	mTorsoSkeNode = mLeftHandSkeNode = mRightHandSkeNode
		= mLeftLegSkeNode = mRightLegSkeNode = -1;
	mMesh = mesh;
	mMeshSkeleton = mMesh->getSkeleton();
}

void HumanSegmenter::handleHead( LevelSet_ levelSet )
{
	if(levelSet->getCount() != 1){
		PRINTLN("Human Segment Error: incorrect categories of head");
		return ;
	}
	addToRegion(mHead, levelSet->getCircle(0));
	if(mHead->hasStartSetted() == false){
		mHead->setPossibleStart(*(mHead->getSkeNodes().begin()));
	}
}

void HumanSegmenter::handleTorso( LevelSet_ levelSet )
{
	if(levelSet->getCount() != 3){
		PRINTLN("Human Segment Error: incorrect categories of torso");
		return ;
	}
	if(mTorsoSkeNode == -1){
		/* compute mLeftHandSkeNode, mTorsoSkeNode, mRightHandSkeNode */
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
		mLeftHandSkeNode = getCircleSkeletonNode(ccp[0].first);
		mTorsoSkeNode = getCircleSkeletonNode(ccp[1].first);
		mRightHandSkeNode = getCircleSkeletonNode(ccp[2].first);
		addToRegion(mLeftHand, ccp[0].first);
		addToRegion(mTorso, ccp[1].first);
		addToRegion(mRightHand, ccp[2].first);
		mLeftHand->setPossibleStart(*(mLeftHand->getSkeNodes().begin()));
		mTorso->setPossibleStart(*(mTorso->getSkeNodes().begin()));
		mRightHand->setPossibleStart(*(mRightHand->getSkeNodes().begin()));
	}
	else{			
		for(size_t i = 0; i < 3; i++){
			LevelCircle_ lc = levelSet->getCircle(i);
			size_t skeNode = getCircleSkeletonNode(lc);
			size_t disLH = mMeshSkeleton->intervalNodeCount(skeNode, mLeftHandSkeNode);
			size_t disTS = mMeshSkeleton->intervalNodeCount(skeNode, mTorsoSkeNode);
			size_t disRH = mMeshSkeleton->intervalNodeCount(skeNode, mRightHandSkeNode);
			if(disLH <= disTS && disLH <= disRH){
				addToRegion(mLeftHand, lc);
				mLeftHandSkeNode = skeNode;					
			}
			else if(disTS <= disLH && disTS <= disRH){
				addToRegion(mTorso, lc);
				mTorsoSkeNode = skeNode;
			}
			else if(disRH <= disTS && disRH <= disLH){
				addToRegion(mRightHand, lc);
				mRightHandSkeNode = skeNode;
			}
		}
	}
}

void HumanSegmenter::handleLegs( LevelSet_ levelSet )
{
	if(levelSet->getCount() != 2){
		PRINTLN("Human Segment Error: incorrect categories of legs");
		return ;
	}
	LevelCircle_ leg1 = levelSet->getCircle(0);
	LevelCircle_ leg2 = levelSet->getCircle(1);
	size_t skeNode1 = getCircleSkeletonNode(leg1);
	size_t skeNode2 = getCircleSkeletonNode(leg2);
	if(mLeftLegSkeNode == -1){
		double c1 = leg1->getCenterX(mMesh);
		double c2 = leg2->getCenterX(mMesh);
		if(c1 < c2){
			mLeftLegSkeNode = skeNode1;
			mRightLegSkeNode = skeNode2;
			addToRegion(mLeftLeg, leg1);
			addToRegion(mRightLeg, leg2);
		}
		else{
			mLeftLegSkeNode = skeNode2;
			mRightLegSkeNode = skeNode1;				
			addToRegion(mLeftLeg, leg2);
			addToRegion(mRightLeg, leg1);
		}
		mLeftLeg->setPossibleStart(*(mLeftLeg->getSkeNodes().begin()));
		mRightLeg->setPossibleStart(*(mRightLeg->getSkeNodes().begin()));
	}
	else{
		size_t dis1 = mMeshSkeleton->intervalNodeCount(skeNode1, mLeftLegSkeNode);
		size_t dis2 = mMeshSkeleton->intervalNodeCount(skeNode1, mRightLegSkeNode);
		if(dis1 < dis2){
			addToRegion(mLeftLeg, leg1);
			addToRegion(mRightLeg, leg2);
			mLeftLegSkeNode = skeNode1;
			mRightLegSkeNode = skeNode2;
		}
		else{
			addToRegion(mLeftLeg, leg2);
			addToRegion(mRightLeg, leg1);
			mLeftLegSkeNode = skeNode2;
			mRightLegSkeNode = skeNode1;
		}
	}
}
