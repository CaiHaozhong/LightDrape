#include <OpenMesh/Core/IO/MeshIO.hh>
#include "HumanSegmenter.h"
#include "Config.h"
#include "LeftTorseRightRefiner.h"
#include "LeftTorseRightSimpleRefiner.h"
#include "MeshSegmentListener.h"
#include "Human.h"
#include <queue>

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
		if(levelSet->getCount() == 1){
			handleTorso(levelSet);
		}
		else if(levelSet->getCount() == 2){
			handleLegs(levelSet);
		}		
	}
	else{
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
	mHead = std::make_shared<Region>(mMesh, "head");
	mTorso = std::make_shared<Region>(mMesh,"torse");
	mLeftHand = std::make_shared<Region>(mMesh, "lefthand");
	mRightHand = std::make_shared<Region>(mMesh, "righthand");
	mLeftLeg = std::make_shared<Region>(mMesh, "leftleg");
	mRightLeg = std::make_shared<Region>(mMesh, "rightleg");
	seg->addRegionRaw(Segment::BODY_HEAD, mHead);
	seg->addRegionRaw(Segment::BODY_TORSE, mTorso);
	seg->addRegionRaw(Segment::BODY_LEFT_HAND, mLeftHand);
	seg->addRegionRaw(Segment::BODY_RIGHT_HAND, mRightHand);
	seg->addRegionRaw(Segment::BODY_LEFT_LEG, mLeftLeg);
	seg->addRegionRaw(Segment::BODY_RIGHT_LEG, mRightLeg);

	mLeftHand->setColor(Vec3uc(200, 0, 0));
	mRightHand->setColor(Vec3uc(0, 200, 0));
	mLeftLeg->setColor(Vec3uc(200, 200, 0));
	mRightLeg->setColor(Vec3uc(0, 200, 200));
	mTorso->setColor(Vec3uc(100, 100, 100));
	mHead->setColor(Vec3uc(100, 100, 100));

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
		std::vector<Region_> curRegions;
		curRegions.push_back(mTorso);
		handleNoise(levelSet, curRegions);
		PRINTLN("Human Segment Error: incorrect categories of head");
		return ;
	}
	LevelCircle_ lc = levelSet->getCircle(0);
	addToRegion(mTorso, lc);
// 
// 	if(isHead(lc)){
// 		addToRegion(mHead, lc);
// 	}
// 	else{
// 		addToRegion(mTorso, lc);
// 	}
}

void HumanSegmenter::handleTorso( LevelSet_ levelSet )
{
// 	if(levelSet->getCount() == 1){
// 		//mTorso->addCircle(levelSet->getCircle(0));
// 		addToRegion(mTorso, levelSet->getCircle(0));
// 		return;
// 	}
	if(levelSet->getCount() != 3){
		std::vector<Region_> curRegions;
		curRegions.push_back(mTorso);
		curRegions.push_back(mLeftHand);
		curRegions.push_back(mRightHand);
		handleNoise(levelSet, curRegions);
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
		std::vector<Region_> curRegions;
		curRegions.push_back(mLeftLeg);
		curRegions.push_back(mRightLeg);
		handleNoise(levelSet, curRegions);
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
	for(auto it = mListeners.begin(); it != mListeners.end(); it++){
		(*it)->onEndCoarseSegment(mSegment);
	}

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
	return;
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
	Config_ config = Config::getInstance();
	if(config->humanRefiner){
		LeftTorseRightRefiner_ refiner = std::make_shared<LeftTorseRightRefiner>(
			shared_from_this(),
			mLeftHand,
			mTorso,
			mRightHand
			);
		refiner->refine();
	}
	if(config->humanSimpleRefiner){
		LeftTorseRightRefiner_ simpleRefiner = std::make_shared<LeftTorseRightSimpleRefiner>(
			shared_from_this(),
			mLeftHand,
			mTorso,
			mRightHand
			);
		simpleRefiner->refine();
	}
// 
// 	mLeftHand->dumpRegionSkeleton(mMesh->getName() + "_lefthand");
// 	mRightHand->dumpRegionSkeleton(mMesh->getName() + "_righthand");
// 	mTorso->dumpRegionSkeleton(mMesh->getName() + "_torse");
}

void HumanSegmenter::refineLegs()
{
	double x = 0, y = 0, z = 0;
	RegionSkeleton_ regionSke = mTorso->getRegionSkeleton();
	for(size_t i = 0; i < regionSke->count(); i++){
		Vec3d& center = regionSke->getNode(i)->center;
		x += center.values_[0];
		z += center.values_[2];
	}
	x /= regionSke->count();
	z /= regionSke->count();
	/* 计算y值，位于大腿顶端 */
	y = -10000;
	Region_ legs[] = {mLeftLeg, mRightLeg};
	int count = 2;
	for(int i = 0; i < 2; i++){
		std::set<size_t>& vers = legs[i]->getVertices();
		for(auto it = vers.begin(); it != vers.end(); it++){
			Vec3d& p = mMesh->point(Mesh::VertexHandle(*it));
			if(p.values_[1] > y){
				y = p.values_[1];
			}
		}
	}
	Vec2d p0(x, y);
	Human_ mHuman = std::static_pointer_cast<Human>(mMesh);
	double uplegWidth = mHuman->getUpperLegWidth();
	double udbHeight = mHuman->getUnderBellyHeight();
	Vec2d p1(x - uplegWidth, y + udbHeight);
	Vec2d p2(x + uplegWidth, y + udbHeight);
	std::vector<size_t> addLeftLeg, addRightLeg;
	pointsUnderLine(addLeftLeg, p0, p1, mLeftLeg, mRightLeg);
	pointsUnderLine(addRightLeg, p0, p2, mRightLeg, mLeftLeg);

// 	/* 计算两腿之间的中心点的横坐标 */
// 	double leftLegMaxX = -10000, rightLegMinX = 10000;	
// 	std::set<size_t>& leftLegVers = mLeftLeg->getVertices();
// 	for(auto it = leftLegVers.begin(); it != leftLegVers.end(); it++){
// 		Vec3d& p = mMesh->point(Mesh::VertexHandle(*it));
// 		if(p.values_[0] > leftLegMaxX){
// 			leftLegMaxX = p.values_[0];
// 		}
// 	}
// 	std::set<size_t>& rightLegVers = mRightLeg->getVertices();
// 	for(auto it = rightLegVers.begin(); it != rightLegVers.end(); it++){
// 		Vec3d& p = mMesh->point(Mesh::VertexHandle(*it));
// 		if(p.values_[0] < rightLegMinX){
// 			rightLegMinX = p.values_[0];
// 		}
// 	}
// 	double legCenterX = (leftLegMaxX + rightLegMinX) / 2;
	Config_ config = Config::getInstance();
	for(size_t i = 0; i < addLeftLeg.size(); i++){
		size_t idx = addLeftLeg[i];
		if(mMesh->point(Mesh::VertexHandle(idx)).values_[0] < x + config->humanLegCenterXep)
			mLeftLeg->addVertex(idx);
	}
	for(size_t i = 0; i < addRightLeg.size(); i++){
		size_t idx = addRightLeg[i];
		if(mMesh->point(Mesh::VertexHandle(idx)).values_[0] > x + config->humanLegCenterXep)
			mRightLeg->addVertex(idx);
	}
	LeftTorseRightRefiner::regionSub(mTorso, mLeftLeg);
	LeftTorseRightRefiner::regionSub(mTorso, mRightLeg);
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

/* ay + bx = c */
void HumanSegmenter::pointsUnderLine( std::vector<size_t>& ret, 
									 const Vec2d& p1, const Vec2d& p2, const Region_ region,
									 const Region_ exclude)
{
	double a = 1.0 / (p2.values_[1] - p1.values_[1]);
	double b = -1.0 / (p2.values_[0] - p1.values_[0]);
	double c = p1.values_[1] / (p2.values_[1] - p1.values_[1])
		- p1.values_[0] / (p2.values_[0] - p1.values_[0]);
	
	std::unordered_set<size_t> testInExclude;
	std::set<size_t>& excludeVers = exclude->getVertices();
	testInExclude.reserve(excludeVers.size());
	for(auto it = excludeVers.begin(); it != excludeVers.end(); it++){
		testInExclude.insert(*it);
	}
	std::set<size_t>& vers = region->getVertices();
	std::unordered_set<size_t> testInRegion;
	testInRegion.reserve(vers.size());
	for(auto it = vers.begin(); it != vers.end(); it++){
		testInRegion.insert(*it);
	}
	std::unordered_set<size_t> isVisited;
	isVisited.reserve(vers.size() * 1.2);
	std::queue<size_t> Q;
	size_t start = *(vers.begin());
	Q.push(start);
	isVisited.insert(start);
	while(! (Q.empty())){
		size_t cur = Q.front();
		Q.pop();
		Mesh::VertexHandle vh(cur);
		for(auto vv_it = mMesh->vv_begin(vh); vv_it.is_valid(); vv_it++){
			size_t idx = vv_it->idx();
			if(isVisited.find(idx) == isVisited.end()){ //未被访问过
				isVisited.insert(idx);
				if(testInExclude.find(idx) != testInExclude.end()){ // 处于排除区域
					continue;
				}
				if(testInRegion.find(idx) != testInRegion.end()) //在区域内
					Q.push(idx);
				else{
					Vec3d& p = mMesh->point(*vv_it);
					double left = a * p.values_[1] + b * p.values_[0];
					if(left < c){
						ret.push_back(idx);
						Q.push(idx);
					}
				}
			}
		}
	}
}
