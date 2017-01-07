#include "VertexAlter.h"


VertexAlter::VertexAlter(void)
{
	trans = Vec3d(0,0,0);
}


VertexAlter::~VertexAlter(void)
{
}

size_t VertexAlter::add( size_t index, const Vec3d& delta )
{
	vertexDelta.push_back(std::make_pair(index, delta));
	return vertexDelta.size();
}

void VertexAlter::translate( const Vec3d& delta )
{
	this->trans = delta;
}
