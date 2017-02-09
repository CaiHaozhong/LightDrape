#include "LaplacianLocalDeformer.h"
#include "LaplacianMeshEditor.h"
#include "VertexAlter.h"
#include <unordered_map>
#include "Mesh.h"
LaplacianLocalDeformer::LaplacianLocalDeformer(void)
{
}


LaplacianLocalDeformer::~LaplacianLocalDeformer(void)
{
}

void LaplacianLocalDeformer::deformMesh( Mesh_ mesh, VertexAlter_ vertexAlter )
{
	std::unordered_map<size_t, std::pair<size_t, Vec3d> > sparseMapDense;
	std::vector<size_t> denseMapSparse;
	size_t alterCount = vertexAlter->size();
	size_t denseCount = 0;
	for(size_t i = 0; i < alterCount; i++){
		size_t v = vertexAlter->getIndex(i);
		const Vec3d& delta = vertexAlter->getDelta(i);
		if(sparseMapDense.find(v) == sparseMapDense.end()){
			sparseMapDense[v] = std::make_pair(denseCount++, delta);
		}
		else{
			sparseMapDense[v].second += delta;
		}
	}
	denseMapSparse.resize(denseCount);
	for(auto it = sparseMapDense.begin(); it != sparseMapDense.end(); it++){
		denseMapSparse[it->second.first] = it->first;
	}
	std::vector<LaplacianMeshEditorVertex> inVertices(denseCount);
	std::vector<LaplacianMeshEditorFace> inFaces;
	std::vector<LaplacianMeshEditorVertex> deltaVertices(denseCount);
	for(size_t i = 0; i < denseCount; i++){
		Vec3d& point = mesh->point(Mesh::VertexHandle(	denseMapSparse[i]));
		Vec3d& delta = point + sparseMapDense[denseMapSparse[i]].second;
		inVertices[i] = LaplacianMeshEditorVertex(
			point.values_[0],
			point.values_[1],
			point.values_[2]
			);
		deltaVertices[i] = LaplacianMeshEditorVertex(
			delta.values_[0], 
			delta.values_[1],
			delta.values_[2]);
	}
	for(Mesh::FaceIter f_it = mesh->faces_begin();
		f_it != mesh->faces_end(); f_it++){
			bool isFaceInRegion = true;
			LaplacianMeshEditorFace laplacianFace;
			for(Mesh::FaceVertexIter fv_it = mesh->fv_begin(*f_it);
				fv_it.is_valid(); fv_it++){
					size_t vertex = fv_it->idx();
					if(sparseMapDense.find(vertex) == sparseMapDense.end()){
						isFaceInRegion = false;
						break;
					}
					laplacianFace.v.push_back(sparseMapDense[vertex].first);
			}
			if(isFaceInRegion){
				inFaces.push_back(laplacianFace);
			}			
	}
	LaplacianMeshEditor lme(&inFaces, &inVertices);
	lme.setDeltaVertices(&deltaVertices);
	std::vector<LaplacianMeshEditorVertex>* resultVertices = lme.compute();
	assert(resultVertices->size() == denseCount);
	for(size_t i = 0; i < denseCount; i++){
		size_t sparseVertex = denseMapSparse[i];
		Vec3d& p = mesh->point(Mesh::VertexHandle(sparseVertex));
		p += (*resultVertices)[i].toOpenMeshVector();
	}
	delete resultVertices;
}
