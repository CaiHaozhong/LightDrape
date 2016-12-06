#include <vector>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include "MeshContainer.h"

bool MeshContainer::importHumanMesh( std::istream& is )
{
	if(importMesh(is, mHumanMesh) == false)
		return false;	
	mHumanMesh.setType(Mesh::TYPE_HUMAN);	
	return true;
}

bool MeshContainer::importClothMesh( std::istream& is )
{
	if(importMesh(is, mClothMesh) == false)
		return false;
	mClothMesh.setType(Mesh::TYPE_CLOTH);
	return true;
}

bool MeshContainer::importMesh( std::istream& is, Mesh& mesh)
{
	mesh.request_vertex_normals();
	OpenMesh::IO::Options opt;
	if(OpenMesh::IO::read_mesh(mesh, is, std::string("obj"), opt) == false){
		return false;
	}
	if ( !opt.check( OpenMesh::IO::Options::VertexNormal ) ){
		// we need face normals to update the vertex normals
		mesh.request_face_normals();
		// let the mesh update the normals
		mesh.update_normals();
		// dispose the face normals, as we don't need them anymore
		mesh.release_face_normals();
	}
	return true;
}

Mesh& MeshContainer::getHumanMesh()
{
	return mHumanMesh;
}

Mesh& MeshContainer::getClothMesh()
{
	return mClothMesh;
}

/* 初始化全局变量 */
MeshContainer globalMeshContatiner;