#include <OpenMesh/Core/IO/MeshIO.hh>
#include "MeshWriter.h"
#include "Garment.h"
#include "Config.h"
#include <fstream>


MeshWriter::MeshWriter(void)
{
}


MeshWriter::~MeshWriter(void)
{
}

bool MeshWriter::writeGarment( Garment_ gar, std::string fileName)
{
	Config_ config = Config::getInstance();
	bool suc = writeMesh(gar, config->clothOutPath, fileName);
	return suc;
}

bool MeshWriter::writeMesh( Mesh_ mesh, std::string path, std::string fileName )
{
	/* Output */
	OpenMesh::IO::Options opt;
	opt += OpenMesh::IO::Options::VertexTexCoord;
	opt += OpenMesh::IO::Options::FaceTexCoord;
	bool suc = OpenMesh::IO::write_mesh(*mesh, path+fileName, opt);
	return suc;
}

char* MeshWriter::readToBuffer( std::string path, std::string fileName )
{
	std::ifstream in(path+fileName, std::ios::binary);
	std::filebuf *pbuf;
	long size;
	char* ret;
	pbuf = in.rdbuf();
	in.close();
}
