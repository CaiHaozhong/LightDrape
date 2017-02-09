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

	/* ƥ������Region�������ط�װRegion�Ķ���λ�� 
	 * �������⣺���·���һ��Region���������һ��Region��
	 * �����������һ��Region */
	virtual VertexAlter_ fit(Region_ humanRegion) = 0;

	virtual std::string getName() = 0;
	void setGarmentRegion(Region_ garmentRegion);
};
S_PTR(RegionFitter);
