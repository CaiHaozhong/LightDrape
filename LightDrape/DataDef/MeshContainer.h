#pragma once
#include "Mesh.h"
#include <fstream>
/*
 * ȫ�ֿɷ��ʵ��࣬�洢����ģ�ͺ��·�ģ��
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

/* ����ȫ�ֱ��� */
extern MeshContainer globalMeshContatiner;
