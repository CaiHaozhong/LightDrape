#include "SkeletonContainer.h"
#include "SkeletonUtility.h"
#include "SkeletonPipeLine.h"


SkeletonContainer::SkeletonContainer(void){}


SkeletonContainer::~SkeletonContainer(void){}

bool SkeletonContainer::addSkeletonFromMesh( const Mesh& mesh )
{
	Skeletonization::Skeleton cgalSkeleton;	
	SkeletonExtractor extractor;
	extractor.extract(mesh, cgalSkeleton);
	Skeleton* skeleton = Skeleton::fromCGALSkeleton(cgalSkeleton);
	SkeletonPipeLine pipeline(skeleton);
	pipeline.excute();
	mSkeletonList.push_back(*skeleton);
	delete skeleton;
	return true;
}

Skeleton& SkeletonContainer::getSkeletonRef( int i )
{
	assert(i < size());
	return mSkeletonList[i];
}

int SkeletonContainer::size()
{
	return mSkeletonList.size();
}

SkeletonContainer globalSkeletonContainer;