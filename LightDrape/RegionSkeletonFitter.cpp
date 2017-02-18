#include "RegionSkeletonFitter.h"
#include "Region.h"
#include "Quaternion.h"
RegionSkeletonFitter::RegionSkeletonFitter(void)
{
	mGarmentRegion = nullptr;
	mHumanRegion = nullptr;
}

RegionSkeletonFitter::RegionSkeletonFitter( Region_ garmentRegion ):RegionFitter(garmentRegion)
{
	mHumanRegion = nullptr;
}


RegionSkeletonFitter::~RegionSkeletonFitter(void)
{
}

VertexAlter_ RegionSkeletonFitter::fit( Region_ humanRegion )
{
	mHumanRegion = humanRegion;
	std::vector< std::pair<size_t, Vec3d> > garSkeTrans;
	std::vector< std::pair<size_t, Quaternion_ > > garSkeRotate;
	fitSkeleton(garSkeTrans, garSkeRotate);
	VertexAlter_ ret = smartNew(VertexAlter);
	//computeTranslation(ret, mGarmentRegion->getMesh(), cicleDeltas, circleHeights);
	return ret;
}

std::string RegionSkeletonFitter::getName()
{
	return "RegionSkeletonFitter";
}

void RegionSkeletonFitter::fitSkeleton( std::vector< std::pair<size_t, Vec3d> >& garSkeTrans, 
									   std::vector< std::pair<size_t, Quaternion_ > >& garSkeRotate )
{
	if( mGarmentRegion == nullptr || mHumanRegion == nullptr){
		PRINTLN("In SimpleSkeletonFitter, regions should not be null.");
		return;
	}
	RegionSkeleton_ humSkeleton = mHumanRegion->getRegionSkeleton();
	RegionSkeleton_ garSkeleton = mGarmentRegion->getRegionSkeleton();
	size_t humanSkeCount = humSkeleton->count();
	size_t garSkeCount = garSkeleton->count();

	size_t garStartSke = 0, humStartSke = 0;
	garSkeTrans.push_back(std::make_pair(garStartSke,
		humSkeleton->getNode(humStartSke)->center - garSkeleton->getNode(garStartSke)->center));
	double garLen = 0;
	double humLen = 0;
	size_t humCurIndex = 0;
	double cachedLen = 0;
	for(size_t i = 1; i < garSkeCount; i++){
		RegionSkeletonNode_ garCurNode = garSkeleton->getNode(i);
		RegionSkeletonNode_ garlastNode = garSkeleton->getNode(i-1);
		garLen += garCurNode->height - garlastNode->height;
		while(humLen < garLen){
			humCurIndex += 1;
			if(humCurIndex >= humanSkeCount)
				break;
			cachedLen = humSkeleton->getNode(humCurIndex)->height - humSkeleton->getNode(humCurIndex-1)->height;
			humLen += cachedLen;
		}
		if(humCurIndex >= humanSkeCount) // 假设袖子比手臂短
			break;
		Vec3d hp1 = humSkeleton->getNode(humCurIndex-1)->center;//humSkeleton->nodeAt(humOrderSkes[humCurIndex-1])->point;
		Vec3d hp2 = humSkeleton->getNode(humCurIndex)->center;//humSkeleton->nodeAt(humOrderSkes[humCurIndex])->point;
		Vec3d transVec = hp2 + (hp1-hp2) * (humLen-garLen)/cachedLen;
		garSkeTrans.push_back(std::make_pair(i, transVec - garCurNode->center));

		/* 计算骨骼旋转量 */
		Vec3d humanVec = hp2-hp1;
		const Vec3d& garCurPoint = garCurNode->center;//garSkeleton->nodeAt(curSke)->point;
		Vec3d garVec = garCurPoint - garSkeleton->getNode(i-1)->center;//garSkeleton->nodeAt(garOrderedSkes[i-1])->point;
		if(i + 1 < garSkeCount){
			garVec += (garSkeleton->getNode(i+1)->center - garCurPoint);
			garVec *= 0.5;
		}
		Vec3d axis = humanVec % garVec;
		double angle = -acos((humanVec|garVec)/(humanVec.length()*garVec.length()));
		garSkeRotate.push_back(std::make_pair(i, std::make_shared<Quaternion>(axis, angle)));
	}
}

