#pragma once
#include "Common.h"
#include "Watertightmesh.h"
class Garment;
S_PTR(Garment);
class HumanFeature;
S_PTR(HumanFeature);
class GarmentSimulationCallBack;
S_PTR(GarmentSimulationCallBack);
class GarmentPhysicalSimulator;
S_PTR(GarmentPhysicalSimulator);
class MeshSegmentListener;
S_PTR(MeshSegmentListener);
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

	double getUpperLegWidth();

	double getUnderBellyHeight();

	/* 上衣在人体的高度 */
	double getClothHeight();

	/* 裤头在人体的高度 */
	double getTrouserHeight();

	void doSimulate(Garment_ garment);

	void addGarmentSimulationCallBack(GarmentSimulationCallBack_ callBack);

	void addMeshSegmentListener(MeshSegmentListener_ listener);
private:
	HumanFeature_ mFeature;

	GarmentPhysicalSimulator_ mSimulator;

	Garment_ mGarment;

	std::vector<MeshSegmentListener_> mMeshSegmentListeners;

};
S_PTR(Human);