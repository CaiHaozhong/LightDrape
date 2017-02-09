#pragma once
#include <string>
#include "Common.h"
class Region;
S_PTR(Region);
class VertexAlter;
S_PTR(VertexAlter);
class RegionFitter
{
protected:
	Region_ mGarmentRegion;
public:
	RegionFitter(void);
	RegionFitter(Region_ garmentRegion);
	~RegionFitter(void);

	/* 匹配两个Region，并返回服装Region的顶点位移 
	 * 形象的理解：将衣服的一个Region穿到人体的一个Region上
	 * 参数：人体的一个Region */
	virtual VertexAlter_ fit(Region_ humanRegion) = 0;

	virtual std::string getName() = 0;
	void setGarmentRegion(Region_ garmentRegion);
};
S_PTR(RegionFitter);
