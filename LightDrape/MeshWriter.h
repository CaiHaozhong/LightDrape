#pragma once
#include "Common.h"
class Garment;
S_PTR(Garment);
class Mesh;
S_PTR(Mesh);
class MeshWriter
{
public:
	MeshWriter(void);
	~MeshWriter(void);
	static bool writeGarment(Garment_ gar, std::string fileName);

	static bool writeMesh(Mesh_ mesh, std::string path, std::string fileName);

private:
	static char* readToBuffer(std::string path, std::string fileName);
};
S_PTR(MeshWriter);
