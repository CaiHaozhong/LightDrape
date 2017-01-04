#include "SkeletonizationPreCompile.h"
#include <iostream>
#include <vector>
#include "WatertightMesh.h"
#include "HumanSegmenter.h"
#include "Human.h"

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
	std::string InPath;
	std::string InFileName;
	std::string OutPath;
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
			if(type == "InPath"){
				configIn >> InPath;
			}
			else if(type == "InFileName"){
				configIn >> InFileName;
			}
			else if(type == "OutPath"){
				configIn >> OutPath;
			}
		}
	}
};
int main(){
	
	Config config("E:\\Project\\LightDrape\\data\\config");	
	Mesh_ mesh = smartNew(Mesh);
	mesh->setName(config.InFileName.substr(0,config.InFileName.size()-4));
	bool readSuc = OpenMesh::IO::read_mesh(*mesh, config.InPath + config.InFileName);
	if(readSuc){
		mesh->request_vertex_normals();
		mesh->request_face_normals();	
	}
	Human_ human = std::make_shared<Human>(mesh);
	PRINTLN("Begin Skeletonize...");
	MeshSkeletonization_ skeletonizer = smartNew(MeshSkeletonizationCached);
	skeletonizer->skeletonize(human);
	//human->dumpSkeLinkMesh();
	PRINTLN("End Skeletonize...");
	HumanSegmenter segmenter(human);
	segmenter.segment();
	Segment_ seg = human->getSegment();
	char* outSegName[] = {"leftHand","rightHand","leftLeg","rightLeg","head","torso"};
	std::vector<std::pair<size_t, Region_> > regions = seg->getRegionsRaw();
	for(size_t i = 0; i < regions.size(); i++){
		std::pair<size_t, Region_> typeRegionPair = regions[i];
		Region_ re = typeRegionPair.second;
		Mesh out;
		std::set<size_t>& vs = re->getVertices();
		for(std::set<size_t>::iterator it = vs.begin();
			it != vs.end(); it++){
			Vec3d ver = human->point(Mesh::VertexHandle(*it));
			out.add_vertex(ver);
		}
		char of[200];
		sprintf(of,"%s.obj", outSegName[typeRegionPair.first]);
		bool wsuc = OpenMesh::IO::write_mesh(out, config.OutPath+of);
		if(wsuc){
			std::cout << "write successfully of seg " << i << std::endl;
		}
	}
	getchar();
}