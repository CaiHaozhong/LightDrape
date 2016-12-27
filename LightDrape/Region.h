#pragma once
#include <vector>
#include "Common.h"
class Region
{
private:
	std::vector<size_t> mVertics;
	std::vector<size_t> mSkeNodes;
public:
	Region(void);
	~Region(void);

	void addVertex(size_t vertex){
		mVertics.push_back(vertex);
	}

	void addSkeleton(size_t skenode){
		mSkeNodes.push_back(skenode);
	}
};
S_PTR(Region);