#include <OpenMesh/Core/IO/MeshIO.hh>
#include "FrameToOBJFileWriter.h"
#include <string>
#include "MeshFramePool.h"
#include "Mesh.h"

FrameToOBJFileWriter::FrameToOBJFileWriter()
{	
	mFrameCount = 0;
}

FrameToOBJFileWriter::~FrameToOBJFileWriter(void)
{
	out.close();	
}

void FrameToOBJFileWriter::onSimulateBegin()
{
	PRINTLN("Begin Simulation!");
	out.open("../data/frames/frame.fr");
}

void FrameToOBJFileWriter::onFrame( MeshFrame_ frame )
{
	std::cout << mFrameCount << " ";
	bool flag = false;
/*	if(flag)*/
	writeFrameToFile(frame);
	mFrameCount++;
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

void FrameToOBJFileWriter::writeFrameToFile( MeshFrame_ frame)
{	
	const std::vector<Vec3d>& vers = frame->getVertices();
	size_t count = vers.size();
	for(size_t i = 0; i < count; i++){
		const auto& v = vers[i].values_;
		out << v[0] << " " << v[1] << " " << v[2] << " ";
	}
}

