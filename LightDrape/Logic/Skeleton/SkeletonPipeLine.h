#pragma once
#include <vector>
#include "SkeletonProcessor.h"
#include "IsolatedPointEliminator.h"
class SkeletonPipeLine
{
public:
	SkeletonPipeLine(Skeleton* ske){
		mSkeleton = ske;
		mSkeletonProcessorList.push_back(new IsolatedPointEliminator);
	}
	~SkeletonPipeLine(void){
		for (int i = 0; i < mSkeletonProcessorList.size(); i++){
			delete mSkeletonProcessorList.at(i);
		}
		mSkeletonProcessorList.clear();
	}
	void addSkeletonProcessor(SkeletonProcessor* processor){
		mSkeletonProcessorList.push_back(processor);
	}
	void excute(){
		for (int i = 0; i < mSkeletonProcessorList.size(); i++){
			mSkeletonProcessorList.at(i)->process(mSkeleton);
		}
	}
private:
	std::vector<SkeletonProcessor*> mSkeletonProcessorList;
	Skeleton* mSkeleton;
};

