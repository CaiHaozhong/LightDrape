#include "MeshTransformer.h"
#include "Vec3d.h"

MeshTransformer::MeshTransformer(void)
{
}


MeshTransformer::~MeshTransformer(void)
{
}

void MeshTransformer::transform()
{
	Vec3d t = Vec3d(1.6854022400000002, -0.011820000000000164, 0.046564950000000022);
	for(Mesh::VertexIter it = mMesh->vertices_begin();
		it != mMesh->vertices_end(); it++){
			Vec3d& p = mMesh->point(*it);
			p += t;
	}
}
