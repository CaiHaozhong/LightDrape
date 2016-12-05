#pragma once
#include "Mesh.h"
#include <fstream>
/*
 * 全局可访问的类，存储人体模型和衣服模型
 */
class MeshContainer {
public:

	bool importHumanMesh(std::istream& is);

	bool importClothMesh(std::istream& is);

	Mesh& getHumanMesh();

	Mesh& getClothMesh();
private:

	bool importMesh(std::istream& is, Mesh& mesh);

	Mesh mHumanMesh;

	Mesh mClothMesh;
};

/* 声明全局变量 */
extern MeshContainer globalMeshContatiner;
