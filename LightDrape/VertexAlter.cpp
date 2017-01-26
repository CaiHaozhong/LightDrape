#include "VertexAlter.h"


VertexAlter::VertexAlter(void){}


VertexAlter::~VertexAlter(void){}

size_t VertexAlter::add( size_t index, const Vec3d& delta )
{
	vertexDelta.push_back(std::make_pair(index, delta));
	return vertexDelta.size();
}

size_t VertexAlter::size() const
{
	return vertexDelta.size();
}

size_t VertexAlter::getIndex( size_t i ) const
{
	return vertexDelta.at(i).first;
}

Vec3d& VertexAlter::getDelta( size_t i )
{
	return vertexDelta[i].second;
}

std::pair<size_t, Vec3d>& VertexAlter::getAlter( size_t i )
{
	return vertexDelta[i];
}
