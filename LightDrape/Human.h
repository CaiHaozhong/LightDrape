#pragma once
#include "Common.h"
#include "Watertightmesh.h"
#include "Garment.h"
class HumanFeature;
S_PTR(HumanFeature);
class Human :
	public WatertightMesh, public std::enable_shared_from_this<Human>
{
public:
	~Human(void);
	Human(Mesh_ mesh);

	size_t getGeodesicSource();

	Vec3d getAlignPoint();
	
	void dress(Garment_ garment);

	HumanFeature_ getFeature() const;

	void setFeature(HumanFeature_ val);

	double getHumanHeight();

	double getShoulderHeight();

	double getLegLength();
private:
	HumanFeature_ mFeature;
};
S_PTR(Human);