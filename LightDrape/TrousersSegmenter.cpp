#include "TrousersSegmenter.h"
#include "TrousersSegment.h"
#include "Config.h"
#include "LeftTorseRightRefiner.h"
#include <queue>
#include <unordered_set>
#include "MeshSegmentListener.h"

TrousersSegmenter::TrousersSegmenter( WatertightMesh_ mesh ) : MeshSegmenter(mesh)
{
	/* C++构造函数中调用虚函数时是不会调用子类函数的，所以分开调用init.(Java是可以的） */
	initTrousersSegmenter(mesh);
}

TrousersSegmenter::TrousersSegmenter( void )
{

}


TrousersSegmenter::~TrousersSegmenter(void)
{
}

void TrousersSegmenter::init( WatertightMesh_ mesh )
{
	MeshSegmenter::init(mesh);
	initTrousersSegmenter(mesh);
}
void TrousersSegmenter::onDifferentLevelSet( size_t seq, LevelSet_ levelSet )
{
	if(seq == 1){
		if(levelSet->getCount() != 1){
			PRINTLN("Trousers Segment Error: incorrect categories of torso");
			return ;
		}
		addToRegion(mTorso, levelSet->getCircle(0));
	}
	else if(seq == 2){
		if(levelSet->getCount() != 2){
			PRINTLN("Trousers Segment Error: incorrect categories of legs");
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
	mCurLevelSet++;
}

void TrousersSegmenter::onFinishCoarseSegment()
{
	MeshSegmenter::onFinishCoarseSegment();
	//----------------------------------------------------------------------------
	auto& skes = mTorso->getSkeNodes();
	for(auto it = skes.begin(); it != skes.end(); it++){
		mMesh->dumpSkeLinkMesh(*it, "torse");
	}
	//----------------------------------------------------------------------------
}

Segment_ TrousersSegmenter::createSegment()
{
	TrousersSegment_ mClothSegment = smartNew(TrousersSegment);
	mTorso = std::make_shared<Region>(mMesh, "torse");
	mLeftLeg = std::make_shared<Region>(mMesh, "leftleg");
	mRightLeg = std::make_shared<Region>(mMesh, "rightleg");
	mClothSegment->addRegion(TrousersSegment::TROUSERS_WAIST, mTorso);
	mClothSegment->addRegion(TrousersSegment::TROUSERS_LEFT, mLeftLeg);
	mClothSegment->addRegion(TrousersSegment::TROUSERS_RIGHT, mRightLeg);			

	mLeftLeg->setColor(Vec3uc(200, 0, 0));
	mRightLeg->setColor(Vec3uc(200, 200, 0));
	mTorso->setColor(Vec3uc(200, 0, 200));
	return mClothSegment;
}

void TrousersSegmenter::initTrousersSegmenter( WatertightMesh_ mesh )
{
	mTorsoSkeNode = mLeftLegSkeNode = mRightLegSkeNode = -1;
	mMesh = mesh;
	mMeshSkeleton = mMesh->getSkeleton();
	mCurLevelSet = 0;
}
void TrousersSegmenter::onFinishSegmentHook()
{
	PRINTLN("End HumanSegment");
	for(auto it = mListeners.begin(); it != mListeners.end(); it++){
		(*it)->onEndCoarseSegment(mSegment);
	}

	dumpRegion(mLeftLeg, Config::getInstance()->clothSegOutPath);
	dumpRegion(mRightLeg, Config::getInstance()->clothSegOutPath);

// 	/* Output Segments */	
// 	Config_ config = Config::getInstance();	
// 	char* outSegNameCloth[] = {"waist", "leftLeg", "rightLeg"};
// 	std::vector<std::pair<int, Region_> > regions = mSegment->getRegionsRaw();
// 	for(size_t i = 0; i < regions.size(); i++){
// 		std::pair<int, Region_> typeRegionPair = regions[i];
// 		Region_ re = typeRegionPair.second;
// 		Mesh out;
// 		std::set<size_t>& vs = re->getVertices();
// 		for(std::set<size_t>::iterator it = vs.begin();
// 			it != vs.end(); it++){
// 				Vec3d ver = mMesh->point(Mesh::VertexHandle(*it));
// 				out.add_vertex(ver);
// 		}
// 		char of[200];
// 		sprintf(of,"%s_%s.obj", mMesh->getName().c_str(), outSegNameCloth[typeRegionPair.first]);
// 		bool wsuc = OpenMesh::IO::write_mesh(out, config->clothSegOutPath+of);
// 		if(wsuc){
// 			std::cout << "write successfully of Trousers seg " << i << std::endl;
// 		}
// 	}
// 	return ;
}

void TrousersSegmenter::onBeginSegmentHook()
{
	PRINTLN("Begin HumanSegment");
}

void TrousersSegmenter::refineSegment()
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
	double uplegWidth = 1.818;
	double udbHeight = 1.88;
	Vec2d p1(x - uplegWidth, y + udbHeight);
	Vec2d p2(x + uplegWidth, y + udbHeight);
	std::vector<size_t> addLeftLeg, addRightLeg;
	pointsUnderLine(addLeftLeg, p0, p1, mLeftLeg, mRightLeg);
	pointsUnderLine(addRightLeg, p0, p2, mRightLeg, mLeftLeg);
	for(size_t i = 0; i < addLeftLeg.size(); i++){
		size_t idx = addLeftLeg[i];
		if(mMesh->point(Mesh::VertexHandle(idx)).values_[0] < x)
			mLeftLeg->addVertex(idx);
	}
	for(size_t i = 0; i < addRightLeg.size(); i++){
		size_t idx = addRightLeg[i];
		if(mMesh->point(Mesh::VertexHandle(idx)).values_[0] > x)
			mRightLeg->addVertex(idx);
	}
	LeftTorseRightRefiner::regionSub(mTorso, mLeftLeg);
	LeftTorseRightRefiner::regionSub(mTorso, mRightLeg);
}
/* ay + bx = c */
void TrousersSegmenter::pointsUnderLine( std::vector<size_t>& ret, 
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
