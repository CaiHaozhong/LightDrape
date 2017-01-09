#include "SkeletonizationPreCompile.h"
#include <iostream>
#include <vector>
#include "WatertightMesh.h"
#include "HumanSegmenter.h"
#include "ClothSegmenter.h"
#include "Human.h"
#include "Cloth.h"
#include "MeshTransformer.h"

int mainForSkeleton(){
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
	return 0;
}
class Config{
public:
	std::string humanInPath;	
	std::string clothInPath;
	std::string humanInFileName;
	std::string clothInFileName;
	std::string clothOutPath;
	Config(){}
	Config(char* file){
		init(file);
	}
	void init(char* file){
		std::string type;
		std::string ignore;
		std::ifstream configIn(file);
		while(!configIn.eof()){
			configIn >> type;
			if(type == "#"){
				getline(configIn, ignore);
				continue;
			}
			if(type == "human_in_path"){
				configIn >> humanInPath;
			}
			else if(type == "cloth_in_path"){
				configIn >> clothInPath;
			}
			else if(type == "human_in_file"){
				configIn >> humanInFileName;
			}
			else if(type == "cloth_in_file"){
				configIn >> clothInFileName;
			}
			else if(type == "cloth_out_path"){
				configIn >> clothOutPath;
			}
		}
	}
};
int main(){	
	Config config("D:\\Develop\\project\\LightDrape\\C++\\data\\config");

	/* Human */
	Mesh_ hRawmesh = smartNew(Mesh);
	hRawmesh->setName(config.humanInFileName.substr(0,config.humanInFileName.size()-4));
	bool readSuc = OpenMesh::IO::read_mesh(*hRawmesh, config.humanInPath + config.humanInFileName);
	if(readSuc){
		hRawmesh->request_vertex_normals();
		hRawmesh->request_face_normals();	
		hRawmesh->requestAABB();
	}	
	Human_ human = std::make_shared<Human>(hRawmesh);

	/* Garment */
	Mesh_ gRawmesh = smartNew(Mesh);
	gRawmesh->setName(config.clothInFileName.substr(0,config.clothInFileName.size()-4));
	readSuc = OpenMesh::IO::read_mesh(*gRawmesh, config.clothInPath + config.clothInFileName);
	if(readSuc){
		gRawmesh->request_vertex_normals();
		gRawmesh->request_face_normals();	
		gRawmesh->requestAABB();
	}	
	Garment_ garment = std::make_shared<Cloth>(gRawmesh);

	/* Dress */
	human->dress(garment);
	
// 	MeshTransformer_ t = std::make_shared<MeshTransformer>(garment);
// 	t->transform();

	/* Output */
	bool suc = OpenMesh::IO::write_mesh(*garment, config.clothOutPath+config.clothInFileName);
	if(suc){
		PRINTLN("write succsss!");
	}
	else{
		PRINTLN("write fail!");
	}
	
	
// 	Segment_ seg = watertightMesh->getSegment();
// 	char* outSegNameHuman[] = {"leftHand","rightHand","leftLeg","rightLeg","head","torso"};
// 	char* outSegNameCloth[] = {"torso", "leftSleeves", "rightSleeves"};
// 	std::vector<std::pair<size_t, Region_> > regions = seg->getRegionsRaw();
// 	for(size_t i = 0; i < regions.size(); i++){
// 		std::pair<size_t, Region_> typeRegionPair = regions[i];
// 		Region_ re = typeRegionPair.second;
// 		Mesh out;
// 		std::set<size_t>& vs = re->getVertices();
// 		for(std::set<size_t>::iterator it = vs.begin();
// 			it != vs.end(); it++){
// 			Vec3d ver = watertightMesh->point(Mesh::VertexHandle(*it));
// 			out.add_vertex(ver);
// 		}
// 		char of[200];
// 		sprintf(of,"%s.obj", outSegNameCloth[typeRegionPair.first]);
// 		bool wsuc = OpenMesh::IO::write_mesh(out, config.OutPath+of);
// 		if(wsuc){
// 			std::cout << "write successfully of seg " << i << std::endl;
// 		}
// 	}
	getchar();
}