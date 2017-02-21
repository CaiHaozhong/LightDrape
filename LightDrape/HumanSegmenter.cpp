#include <OpenMesh/Core/IO/MeshIO.hh>
#include "HumanSegmenter.h"
#include "Config.h"
#include "LeftTorseRightRefiner.h"
#include "LeftTorseRightSimpleRefiner.h"

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
	mCurLevelSet++;
}

void HumanSegmenter::onFinishCoarseSegment()
{
	MeshSegmenter::onFinishCoarseSegment();
}

Segment_ HumanSegmenter::createSegment()
{
	Segment_ seg = smartNew(Segment);
	mHead = std::make_shared<Region>(mMesh);
	mTorso = std::make_shared<Region>(mMesh);
	mLeftHand = std::make_shared<Region>(mMesh);
	mRightHand = std::make_shared<Region>(mMesh);
	mLeftLeg = std::make_shared<Region>(mMesh);
	mRightLeg = std::make_shared<Region>(mMesh);
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
	mIsHead = true;
	mLastLCRadius = -1;
	mCurLevelSet = 0;
}

void HumanSegmenter::handleHead( LevelSet_ levelSet )
{
	if(levelSet->getCount() != 1){
		PRINTLN("Human Segment Error: incorrect categories of head");
		return ;
	}
	LevelCircle_ lc = levelSet->getCircle(0);
	if(isHead(lc)){
		addToRegion(mHead, lc);
	}
	else{
		addToRegion(mTorso, lc);
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

bool HumanSegmenter::isHead( LevelCircle_ lc )
{
	if(mIsHead == false){
		return false;
	}
// 	if(mLastLCRadius < 0){
// 		mLastLCRadius = maxRadius(lc);
// 		return true;
// 	}
// 	double curRadius = maxRadius(lc);
// 	double minBetweenTwo = std::min(mLastLCRadius, curRadius);
// 	if(abs(curRadius-mLastLCRadius) > 0.65 * minBetweenTwo){
// 		mIsHead = false;
// 		return false;
// 	}
// 	mLastLCRadius = curRadius;

	if(mCurLevelSet >= 6/*getLevelSetCount() / 7.0*/){
		mIsHead = false;
		return false;
	}
	return true;
}

double HumanSegmenter::maxRadius( LevelCircle_ lc )
{
	double minD = std::numeric_limits<double>::min();
	double maxD = std::numeric_limits<double>::max();
	Vec3d minP = Vec3d(maxD, maxD, maxD);
	Vec3d maxP = Vec3d(minD, minD, minD);
	std::vector<LevelNode_> nodes = lc->levelNodes;
	for(size_t i = 0; i < nodes.size(); i++){
		LevelNode_ n = nodes[i];
		minP.minimized(n->exactlyPoint(mMesh));
		maxP.maximize(n->exactlyPoint(mMesh));
	}
	return (minP-maxP).length() / 2.0;
}

void HumanSegmenter::onFinishSegmentHook()
{
	PRINTLN("End HumanSegment");
	
	/* Output Segments */
	char* outSegNameHuman[] = {"leftHand","rightHand","leftLeg","rightLeg","head","torso"};
	Config_ config = Config::getInstance();
	std::vector<std::pair<int, Region_> > regions = mSegment->getRegionsRaw();
	for(size_t i = 0; i < regions.size(); i++){
		std::pair<int, Region_> typeRegionPair = regions[i];
		Region_ re = typeRegionPair.second;
		Mesh out;
		std::set<size_t>& vs = re->getVertices();
		for(std::set<size_t>::iterator it = vs.begin();
			it != vs.end(); it++){
			Vec3d ver = mMesh->point(Mesh::VertexHandle(*it));
			out.add_vertex(ver);
		}
		char of[200];
		sprintf(of,"%s_%s.obj", mMesh->getName().c_str(), outSegNameHuman[typeRegionPair.first]);
		bool wsuc = OpenMesh::IO::write_mesh(out, config->humanSegOutPath+of);
		if(wsuc){
			std::cout << "write successfully of human seg " << i << std::endl;
		}
	}

}

void HumanSegmenter::onBeginSegmentHook()
{
	PRINTLN("Begin HumanSegment");
}

void HumanSegmenter::refineSegment()
{
	refineHands();
	refineLegs();
}

void HumanSegmenter::refineHands()
{
	LeftTorseRightRefiner_ refiner = std::make_shared<LeftTorseRightSimpleRefiner>(
		shared_from_this(),
		mLeftHand,
		mTorso,
		mRightHand
		);
	refiner->refine();
	mLeftHand->dumpRegionSkeleton(mMesh->getName() + "_lefthand");
	mRightHand->dumpRegionSkeleton(mMesh->getName() + "_righthand");
	mTorso->dumpRegionSkeleton(mMesh->getName() + "_torse");
}

void HumanSegmenter::refineLegs()
{

}

void HumanSegmenter::filterNoise( std::vector<bool>& isNoise )
{
	MeshSegmenter::filterNoise(isNoise);
	size_t levelSetCount = getLevelSetCount();
	for(size_t i = 0; i < levelSetCount; i++){
		if(getLevelSet(i)->getCount() > 3){
			isNoise[i] = true;
		}
	}
}
