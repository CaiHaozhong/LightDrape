#include <OpenMesh/Core/IO/MeshIO.hh>
#include <iostream>
#include "Human.h"
#include "Cloth.h"
#include "MeshTransformer.h"
#include "Config.h"
#include "HumanFeature.h"
int main(){
	Config_ config = Config::getInstance();
	/* Human */
	Mesh_ hRawmesh = smartNew(Mesh);
	hRawmesh->request_vertex_normals();
	OpenMesh::IO::Options opt;
	hRawmesh->setName(config->humanInFileName.substr(0,config->humanInFileName.size()-4));
	bool readSuc = OpenMesh::IO::read_mesh(*hRawmesh, config->humanInPath + config->humanInFileName, opt);
	if(readSuc){
		if ( !opt.check( OpenMesh::IO::Options::VertexNormal ) )
		{
			// we need face normals to update the vertex normals
			hRawmesh->request_face_normals();
			// let the mesh update the normals
			hRawmesh->update_normals();
			// dispose the face normals, as we don't need them anymore
			hRawmesh->release_face_normals();
		}
	}	
	Human_ human = std::make_shared<Human>(hRawmesh);
	HumanFeature_ feature = smartNew(HumanFeature);
	feature->fromMakeHumanMeasureFile(config->humanInPath + human->getName() + ".par");
	human->setFeature(feature);

	MeshTransformerFactory_ meshTransformerFactory = smartNew(AxisYTransFactory);
	MeshTransformer_ transformer = meshTransformerFactory->createTransfomer(config->humanMeshDiretion);
	if(transformer){
		transformer->setMesh(human);
		transformer->transform();
	}

	/* Output */
	//OpenMesh::IO::write_mesh(*human, config->clothOutPath+config->humanInFileName);

	/* Garment */
	Mesh_ gRawmesh = smartNew(Mesh);
	gRawmesh->setName(config->clothInFileName.substr(0,config->clothInFileName.size()-4));
	readSuc = OpenMesh::IO::read_mesh(*gRawmesh, config->clothInPath + config->clothInFileName);
	if(readSuc){
		gRawmesh->request_vertex_normals();
		gRawmesh->request_face_normals();	
		gRawmesh->requestAABB();
	}	
	Garment_ garment = std::make_shared<Cloth>(gRawmesh);

	/* Dress */
	human->dress(gRawmesh);
	
	/* Output */
	bool suc = OpenMesh::IO::write_mesh(*(gRawmesh), config->clothOutPath+config->clothInFileName);
	if(suc){
		PRINTLN("write succsss!");
	}
	else{
		PRINTLN("write fail!");
	}
	
	getchar();

	return 0;
}