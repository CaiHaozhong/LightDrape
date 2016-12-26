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
};
S_PTR(Region);