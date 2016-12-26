#include <iostream>
#include <vector>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include "WatertightMesh.h"
#include "MeshSkeletonization.h"

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
void filterNoise(std::vector<int>& in, std::vector<bool>& isNoise){
	isNoise.resize(in.size(), false);
	size_t len = in.size();
	int threshold = 3; // 连续个数小于等于threshold的都算噪音
	int val = in[0];
	int accu = 1;
	int start = 0;
	for(size_t i = 1; i < len; i++){
		int cur = in[i];
		if(cur == val){
			++accu;
		}
		else{				
			if(accu <= threshold){
				while(accu--){
					isNoise[start++] = true;
				}
			}
			start = i;
			val = cur;
			accu = 1;
		}			
	}
	if(accu <= threshold){
		while(accu--){
			isNoise[start++] = true;
		}
	}
}
int main(){
	int i;
	while(true){
		std::vector<int> in;
		std::vector<bool> isNoise;
		int count;
		std::cout << "count: ";
		std::cin >> count;
		for(int c = 0; c < count; c++){
			std::cin >> i;
			in.push_back(i);
		}
		filterNoise(in, isNoise);
		for(int j = 0; j < in.size(); j++){
			std::cout << isNoise[j] << ' ';
		}
		std::cout << std::endl;
	}
	
}