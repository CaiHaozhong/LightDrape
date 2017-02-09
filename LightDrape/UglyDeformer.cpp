#include "UglyDeformer.h"
#include "VertexAlter.h"
#include "Mesh.h"

UglyDeformer::UglyDeformer(void)
{
}


UglyDeformer::~UglyDeformer(void)
{
}

void UglyDeformer::deformMesh( Mesh_ mesh )
{
	if(mesh == nullptr) return;		
	VertexAlter_ vertexAlter = mesh->getVertexAlter();
	if(vertexAlter == nullptr) return;		

	std::vector<std::pair<size_t, Vec3d> >& alters = vertexAlter->vertexDelta;
	size_t count = alters.size();
	for(size_t i = 0; i < count; i++){
		std::pair<size_t, Vec3d>& a = alters[i];
		mesh->point(Mesh::VertexHandle(a.first)) += a.second;
	}
}
