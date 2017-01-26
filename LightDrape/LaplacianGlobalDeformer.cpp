#include "LaplacianGlobalDeformer.h"
#include "LaplacianMeshEditor.h"
#include "Mesh.h"
#include "VertexAlter.h"

LaplacianGlobalDeformer::LaplacianGlobalDeformer(void)
{
}


LaplacianGlobalDeformer::~LaplacianGlobalDeformer(void)
{
}

void LaplacianGlobalDeformer::deformMesh(Mesh_ mesh, VertexAlter_ vertexAlter)
{
// 	VertexAlter_ alters = mesh->getVertexAlter();
// 	size_t alterCount = alters->vertexDelta.size();
// 	for(Mesh::VertexIter v_it = mesh->vertices_begin();
// 		v_it != mesh->vertices_end(); v_it++){
// 			Vec3d& point = mesh->point(*v_it);
// 			point += alters->trans;
// 	}
// 
// 	std::vector<LaplacianMeshEditorFace> faceList;
// 	std::vector<LaplacianMeshEditorVertex> vertexList;
// 	std::vector<LaplacianMeshEditorVertex> deltaList;
// 	for(Mesh::FaceIter f_it = mesh->faces_begin(); 
// 		f_it != mesh->faces_end(); f_it++){
// 			LaplacianMeshEditorFace f;
// 			for(Mesh::FaceVertexIter fv = mesh->fv_begin(*f_it);
// 				fv.is_valid(); fv++){
// 					f.v.push_back(fv->idx());
// 			}
// 			faceList.push_back(f);
// 	}
// 	for(Mesh::VertexIter v_it = mesh->vertices_begin();
// 		v_it != mesh->vertices_end(); v_it++){
// 			Vec3d& point = mesh->point(*v_it);
// 			vertexList.push_back(LaplacianMeshEditorVertex(
// 				point.values_[0],
// 				point.values_[1],
// 				point.values_[2]
// 				));
// 	}		
// 	size_t vCount = mesh->n_vertices();
// 	deltaList = vertexList;
// 	for(size_t i = 0; i < alterCount; i++){
// 		size_t index = alters->vertexDelta[i].first;
// 		Vec3d& delta = alters->vertexDelta[i].second;
// 		if(index >= vCount){
// 			PRINTLN("InValid delta in LaplacianMeshDeformer");
// 			continue;
// 		}		
// 		deltaList[index].x += delta.values_[0];
// 		deltaList[index].y += delta.values_[1];
// 		deltaList[index].z += delta.values_[2];
// 	}
// 	LaplacianMeshEditor lme(&faceList, &vertexList);
// 	lme.setDeltaVertices(&deltaList);
// 	std::vector<LaplacianMeshEditorVertex>* resultVertices = lme.compute();
// 	for(Mesh::VertexIter v_it = mesh->vertices_begin();
// 		v_it != mesh->vertices_end(); v_it++){
// 			Vec3d& point = mesh->point(*v_it);
// 			LaplacianMeshEditorVertex& newV = (*resultVertices)[v_it->idx()];
// 			point.values_[0] = newV.x;
// 			point.values_[1] = newV.y;
// 			point.values_[2] = newV.z;
// 	}
// 	delete resultVertices;
}
