#include "MeshFramePool.h"
#include "Mesh.h"

MeshFrame::MeshFrame()
{

}
MeshFrame::MeshFrame(std::vector<Vec3d>& points)
{
	initFromPoints(points);
}
MeshFrame::MeshFrame( Mesh_ mesh )
{
	initFromMesh(mesh);
}

void MeshFrame::initFromMesh( Mesh_ mesh )
{
	if(mesh == nullptr){
		return;
	}
	for(auto it = mesh->vertices_begin(); it != mesh->vertices_end(); it++){
		vers.push_back(mesh->point(*it));
	}
}

void MeshFrame::initFromPoints( std::vector<Vec3d>& points )
{
	this->vers = points;
}

std::vector<Vec3d>& MeshFrame::getVertices()
{
	return vers;
}


MeshFramePool::MeshFramePool(void)
{
	mFrameRate = 25;
}


MeshFramePool::~MeshFramePool(void)
{
}

void MeshFramePool::storeFrame( MeshFrame_ frame )
{
	mFrames.push_back(frame);
}

void MeshFramePool::storeFrame(std::vector<Vec3d>& frame)
{
	mFrames.push_back(std::make_shared<MeshFrame>(frame));
}

void MeshFramePool::storeFrame( Mesh_ frame )
{
	mFrames.push_back(std::make_shared<MeshFrame>(frame));
}

void MeshFramePool::setFPS( size_t fps )
{
	mFrameRate = fps;
}

size_t MeshFramePool::getFPS() const
{
	return mFrameRate;
}

MeshFrame_ MeshFramePool::getFrame( size_t index )
{
	if(index >= mFrames.size())
		return nullptr;
	return mFrames[index];
}

size_t MeshFramePool::getFrameCount() const
{
	return mFrames.size();
}


