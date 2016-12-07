#include "KNNSHelper.h"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include "Mesh.h"
#include <fstream>
#include "HoleInfo.h"
using namespace std;
int main(){
	Mesh mesh;
	mesh.request_vertex_normals();
	OpenMesh::IO::Options opt;
	ifstream is = ifstream("E:\\Project\\LightDrape\\data\\cloth1.obj");
	bool suc = OpenMesh::IO::read_mesh(mesh, is, "obj", opt);
	if(!suc){
		cout << "import err!" << "\n";
		getchar();
		return 0;
	}
	if ( !opt.check( OpenMesh::IO::Options::VertexNormal ) ){
		// we need face normals to update the vertex normals
		mesh.request_face_normals();
		// let the mesh update the normals
		mesh.update_normals();
		// dispose the face normals, as we don't need them anymore
		mesh.release_face_normals();
	}
	cout << "import suc!" << "\n";
	HoleInfo holeInfo(&mesh);
	holeInfo.getHoles();
	vector<HoleInfo::Hole>* holes = holeInfo.holes();
	cout << "holes cout: " << holes->size() << "\n";
	holeInfo.fillAllHoles();
	suc = OpenMesh::IO::write_mesh(mesh, ofstream("E:\\Project\\LightDrape\\data\\cloth1closed.obj"), "obj");
	if(!suc){
		cout << "save fail!" << "\n";
	}
	else
		cout << "save suc!" << "\n";
	getchar();
}