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
	for(Mesh::VertexIter it = mesh->vertices_begin();
		it != mesh->vertices_end(); it++){
			Vec3d& p = mesh->point(*it);
			Vec3d t = vertexAlter->trans;
			p += t;
	}
	std::vector<std::pair<size_t, Vec3d> >& alters = vertexAlter->vertexDelta;
	size_t count = alters.size();
	for(size_t i = 0; i < count; i++){
		std::pair<size_t, Vec3d>& a = alters[i];
		mesh->point(Mesh::VertexHandle(a.first)) += a.second;
	}
}
