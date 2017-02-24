#include <OpenMesh/Core/IO/MeshIO.hh>
#include "FrameToOBJFileWriter.h"
#include <string>
#include "MeshFramePool.h"
#include "Mesh.h"


FrameToOBJFileWriter::FrameToOBJFileWriter(Mesh_ mesh)
{
	mMesh = std::make_shared<Mesh>(*mesh);
	mCurFrame = 0;
}


FrameToOBJFileWriter::~FrameToOBJFileWriter(void)
{
}

void FrameToOBJFileWriter::onSimulateBegin()
{
	PRINTLN("Begin Simulation!");
}

void FrameToOBJFileWriter::onFrame( MeshFrame_ frame )
{
	PRINT("New Frame! ");
	writeFrameToFile(frame, mCurFrame++);
}

void FrameToOBJFileWriter::onSimulateEnd( MeshFramePool_ meshFramePool )
{
	PRINTLN("End Simulation!");
// 	size_t frameCount = meshFramePool->getFrameCount();
// 	for(size_t i = 0; i < frameCount; i++){
// 		MeshFrame_ frame = meshFramePool->getFrame(i);
// 		writeFrameToFile(frame, i);
// 	}
//	PRINTLN("End Frames Wirting!");
}

void FrameToOBJFileWriter::writeFrameToFile( MeshFrame_ frame, size_t i )
{
	char name[10];
	sprintf(name, "%d.obj", i);
	std::string outFileName = std::string("../data/frames/") + name;
	const std::vector<Vec3d>& vers = frame->getVertices();
	for(size_t i = 0; i < vers.size(); i++){
		Vec3d& v = mMesh->point(Mesh::VertexHandle(i));
		v = vers[i];
	}
	bool suc = OpenMesh::IO::write_mesh(*mMesh, outFileName);
	if(!suc){
		PRINT_ERROR("In rameToOBJFileWriter::writeFrameToFile, write fail.");
	}
}
