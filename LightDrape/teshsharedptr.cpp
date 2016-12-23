#include <iostream>
#include <vector>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include "WatertightMesh.h"
#include "MeshSkeletonization.h"

int main(){
	Mesh_ m = smartNew(Mesh);
	char* file = "D:\\Develop\\project\\LightDrape\\C++\\data\\upclothwithsleeves\\cloth.obj";
	bool ret = OpenMesh::IO::read_mesh(*m, file);
	WatertightMesh_ wm = nullptr;
	if(ret){
		m->request_face_normals();
		m->request_vertex_normals();
		wm = std::make_shared<WatertightMesh>(m);
		MeshSkeletonization msk;
		msk.skeletonize(wm);
	}
	getchar();
}