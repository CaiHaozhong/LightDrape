#include "MeshMaterial.h"
#include <fstream>

MeshMaterial::MeshMaterial(void)
{
}


MeshMaterial::~MeshMaterial(void)
{
}

std::shared_ptr<MeshMaterial> MeshMaterial::fromMakeHumanMeasureFile( std::string fileName )
{
	MeshMaterial_ ret = nullptr;
	std::ifstream in(fileName);
	std::string token;
	if(in.is_open()){
		ret = smartNew(MeshMaterial);
		while(!in.eof()){
			in >> token;
			if(token == "#"){
				getline(in, token);
				continue;
			}
			else if(token == "newmtl")
				in >> ret->materialName;
			else if(token == "Ns")
				in >> ret->Ns;
			else if(token == "Ni")
				in >> ret->Ni;
			else if(token == "d")
				in >> ret->d;
			else if(token == "Tr")
				in >> ret->Tr;
			else if(token == "Tf")
				in >> ret->Tf[0] >> ret->Tf[1] >> ret->Tf[2];
			else if(token == "illum")
				in >> ret->illum;
			else if(token == "Ka")
				in >> ret->Ka[0] >> ret->Ka[1] >> ret->Ka[2];
			else if(token == "Kd")
				in >> ret->Kd[0] >> ret->Kd[1] >> ret->Kd[2];
			else if(token == "Ks")
				in >> ret->Ks[0] >> ret->Ks[1] >> ret->Ks[2];
			else if(token == "Ke")
				in >> ret->Ke[0] >> ret->Ke[1] >> ret->Ke[2];
			else if(token == "map_Ka")
				in >> ret->map_Ka;
			else if(token == "map_Kd")
				in >> ret->map_Kd;
		}
	}
	else{
		PRINT_ERROR(std::string("Read Material in") + fileName + " fail.");
	}
	return ret;
}
