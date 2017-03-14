#include <OpenMesh/Core/IO/MeshIO.hh>
#include "TrousersSegmenter.h"
#include "TrousersSegment.h"
#include "Config.h"
#include "LeftTorseRightRefiner.h"

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
	mTorso = std::make_shared<Region>(mMesh);
	mLeftLeg = std::make_shared<Region>(mMesh);
	mRightLeg = std::make_shared<Region>(mMesh);
	mClothSegment->addRegion(TrousersSegment::TROUSERS_WAIST, mTorso);
	mClothSegment->addRegion(TrousersSegment::TROUSERS_LEFT, mLeftLeg);
	mClothSegment->addRegion(TrousersSegment::TROUSERS_RIGHT, mRightLeg);				
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

	/* Output Segments */	
	Config_ config = Config::getInstance();	
	char* outSegNameCloth[] = {"waist", "leftLeg", "rightLeg"};
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
		sprintf(of,"%s_%s.obj", mMesh->getName().c_str(), outSegNameCloth[typeRegionPair.first]);
		bool wsuc = OpenMesh::IO::write_mesh(out, config->clothSegOutPath+of);
		if(wsuc){
			std::cout << "write successfully of Trousers seg " << i << std::endl;
		}
	}

}

void TrousersSegmenter::onBeginSegmentHook()
{
	PRINTLN("Begin HumanSegment");
}

void TrousersSegmenter::refineSegment()
{
	LeftTorseRightRefiner::regionSub(mTorso, mLeftLeg);
	LeftTorseRightRefiner::regionSub(mTorso, mRightLeg);
}
