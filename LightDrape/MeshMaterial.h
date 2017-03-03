#pragma once
#include "Common.h"
#include "Vec3d.h"
#include <string>
class MeshMaterial
{
public:
	double Ns;
	double Ni;
	double d;
	double Tr;
	Vec3d Tf;
	double illum;
	double Ka[3];
	double Kd[3];
	double Ks[3];
	double Ke[3];
	std::string map_Ka;
	std::string map_Kd;

	std::string materialName;

	MeshMaterial(void);
	~MeshMaterial(void);
	static std::shared_ptr<MeshMaterial> fromMakeHumanMeasureFile(std::string fileName);
};
S_PTR(MeshMaterial);
