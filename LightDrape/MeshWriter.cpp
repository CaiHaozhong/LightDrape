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
	long size;
	char* buffer = readToBuffer(path, fileName, size);
	Config_ config = Config::getInstance();
	std::ofstream out(path+fileName);
	out << "mtllib " << config->mtlFileName << "\n";
	out << "usemtl " << config->useMtlName << "\n";
	out.write(buffer,size);
	out.close();
	return suc;
}

char* MeshWriter::readToBuffer( std::string path, std::string fileName, long& size )
{
	std::ifstream in(path+fileName, std::ios::binary);
	std::filebuf *pbuf;	
	char* ret;
	pbuf = in.rdbuf();
	// 调用buffer对象方法获取文件大小  
	size = pbuf->pubseekoff (0,std::ios::end,std::ios::in);  
	pbuf->pubseekpos (0,std::ios::in);
	ret = new char[size];
	// 获取文件内容  
	pbuf->sgetn (ret, size);	
	in.close();
	return ret;
}
