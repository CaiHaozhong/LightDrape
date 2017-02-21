#include "Mesher.h"
#include "Region.h"
#include "Mesh.h"
#include "WatertightMesh.h"
#include <set>


Mesher::Mesher(void)
{
}

Mesher::Mesher( Region_ region )
{
	initWithRegion(region);
}


Mesher::~Mesher(void)
{
}

size_t Mesher::getMeshVertex( size_t regionVertex )
{
	if(mSparseMapDense.find(regionVertex) == mSparseMapDense.end()){
		PRINTLN("Region vertex not in regionMesh");
		return 0;
	}
	return mSparseMapDense[regionVertex];
}

size_t Mesher::getRegionVertex( size_t meshVertex )
{
	if(meshVertex >= mDenseMapSparse.size()){
		PRINTLN("MeshVertex out of bound");
		return 0;
	}
	return mDenseMapSparse[meshVertex];
}

void Mesher::initWithRegion( Region_ region )
{
	std::set<size_t>& versInRegion = region->getVertices();
	size_t verCount = versInRegion.size();
	mSparseMapDense.reserve(verCount);
	size_t denseCount = 0;
	for(auto it = versInRegion.begin(); it != versInRegion.end(); it++){
		size_t v = *it;
		if(mSparseMapDense.find(v) == mSparseMapDense.end()){
			mSparseMapDense[v] = denseCount++;
		}
	}
	mDenseMapSparse.resize(denseCount);
	for(auto it = mSparseMapDense.begin(); it != mSparseMapDense.end(); it++){
		mDenseMapSparse[it->second] = it->first;
	}
	mRetMesh = smartNew(Mesh);
	Mesh_ regionMesh = region->getMesh();
	for(size_t i = 0; i < denseCount; i++){
		Vec3d& point = regionMesh->point(Mesh::VertexHandle(	mDenseMapSparse[i]));
		mRetMesh->add_vertex(point);
	}
	for(Mesh::FaceIter f_it = regionMesh->faces_begin();
		f_it != regionMesh->faces_end(); f_it++){
			bool isFaceInRegion = true;
			std::vector<Mesh::VertexHandle> face;
			for(Mesh::FaceVertexIter fv_it = regionMesh->fv_begin(*f_it);
				fv_it.is_valid(); fv_it++){
					size_t vertex = fv_it->idx();
					if(mSparseMapDense.find(vertex) == mSparseMapDense.end()){
						isFaceInRegion = false;
						break;
					}
					face.push_back(Mesh::VertexHandle(mSparseMapDense[vertex]));
			}
			if(isFaceInRegion){
				mRetMesh->add_face(face);
			}
	}
}
