#pragma once
#include "Skeletonprocessor.h"

class IsolatedPointEliminator : public SkeletonProcessor{
public:
	IsolatedPointEliminator(void);
	~IsolatedPointEliminator(void);

	void process(Skeleton* skeleton);
};

