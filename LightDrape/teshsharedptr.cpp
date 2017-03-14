#include <OpenMesh/Core/IO/MeshIO.hh>
#include <iostream>
#include "Human.h"
#include "Cloth.h"

#include "Config.h"
#include "HumanFeature.h"
#include "Visualizer.h"
#include <QApplication>
#include "QGLViewerWidget.h"
#include "MeshWidget.h"
#include <QMainWindow>
#include "MeshLoader.h"
#include "GeodesicResolverCached.h"
#include <QThread>
class MyCallBack : public MeshLoader::MeshLoaderListener{
private:
	Human_ mHuman;
	Garment_ mGarment;
public:
	/* 开始加载所有模型 */
	void onBeginLoad(){
		std::cout << "onBeginLoad" << std::endl;
	};

	/* 开始加载人体模型 */
	void onBeginLoadHuman(){
		std::cout << "onBeginLoadHuman" << std::endl;
	};

	/* 开始加载第i个衣服模型 */
	void onBeginLoadGarment(int i){
		std::cout << "onBeginLoadGarment" << std::endl;
	};

	/* 人体模型加载结束 */
	void onEndLoadHuman(Human_ human){
		mHuman = human;		
		std::cout << "onEndLoadHuman" << std::endl;
	};

	/* 第i个衣服模型加载结束 */
	void onEndLoadGarment(int i, Garment_ gar){
		mGarment = gar;
// 		std::cout << "Resolve " << i << std::endl;
// 		GeodesicResolver_ resolver = smartNew(GeodesicResolverCached);
// 		std::cout << "End Resolve " << i << std::endl;
// 		resolver->resolveGeo(gar);
		std::cout << "onEndLoadGarment" << i <<  std::endl;
	};

	/* 所有模型加载完毕 */
	void onEndLoad(){
		QMainWindow* mainWin = new QMainWindow;	
		MeshWidget* glWidget = new MeshWidget;
		glWidget->setHuman(mHuman);
		glWidget->setGarment(mGarment);
		mainWin->setCentralWidget(glWidget);
		mainWin->resize(640,480);
		mainWin->show();
		std::cout << "onEndLoad" << std::endl;
	};
};
int main(int argc, char** argv){
// 	Config_ config = Config::getInstance();
// 	/* Human */
// 	Mesh_ hRawmesh = smartNew(Mesh);
// 	hRawmesh->request_vertex_normals();
// 	OpenMesh::IO::Options opt;
// 	opt += OpenMesh::IO::Options::VertexNormal;
// 	hRawmesh->setName(config->humanInFileName.substr(0,config->humanInFileName.size()-4));
// 	bool readSuc = OpenMesh::IO::read_mesh(*hRawmesh, config->humanInPath + config->humanInFileName, opt);
// 	if(readSuc){
// 		if ( !opt.check( OpenMesh::IO::Options::VertexNormal ) )
// 		{
// 			// we need face normals to update the vertex normals
// 			hRawmesh->request_face_normals();
// 			// let the mesh update the normals
// 			hRawmesh->update_normals();
// 			// dispose the face normals, as we don't need them anymore
// 			hRawmesh->release_face_normals();
// 		}
// 		hRawmesh->requestAABB();
// 	}	
// 	Human_ human = std::make_shared<Human>(hRawmesh);
// // 	HumanFeature_ feature = smartNew(HumanFeature);
// // 	feature->fromMakeHumanMeasureFile(config->humanInPath + human->getName() + ".par");
// // 	human->setFeature(feature);
// 
// 	MeshTransformerFactory_ meshTransformerFactory = smartNew(AxisYTransFactory);
// 	MeshTransformer_ transformer = meshTransformerFactory->createTransfomer(config->humanMeshDiretion);
// 	if(transformer){
// 		transformer->setMesh(human);
// 		transformer->transform();
// 	}
// 
// 	/* Output */
// 	//OpenMesh::IO::write_mesh(*human, config->clothOutPath+config->humanInFileName);
// 
// 	/* Garment */
// 	Mesh_ gRawmesh = smartNew(Mesh);
// 	gRawmesh->request_vertex_normals();
// 	gRawmesh->setName(config->clothInFileName.substr(0,config->clothInFileName.size()-4));
// 	readSuc = OpenMesh::IO::read_mesh(*gRawmesh, config->clothInPath + config->clothInFileName, opt);
// 	if(readSuc){
// 		if ( !opt.check( OpenMesh::IO::Options::VertexNormal ) )
// 		{
// 			// we need face normals to update the vertex normals
// 			gRawmesh->request_face_normals();
// 			// let the mesh update the normals
// 			gRawmesh->update_normals();
// 			// dispose the face normals, as we don't need them anymore
// 			gRawmesh->release_face_normals();
// 		}
// 		gRawmesh->requestAABB();
// 	}	
// 	Garment_ garment = std::make_shared<Cloth>(gRawmesh);

	QApplication app(argc,argv);
	glutInit(&argc,argv);
	MeshLoader_ mLoader = smartNew(MeshLoader);
	mLoader->addMeshLoaderListener(smartNew(MyCallBack));
	mLoader->asynload();	
	return app.exec();

/* -------------------------------------- End load Mesh ----------------------------------------*/

// 
// 	QApplication app(argc,argv);
// 	glutInit(&argc,argv);
// 	QMainWindow mainWin;	
// 	MeshWidget* glWidget = new MeshWidget;
// 	mainWin.setCentralWidget(glWidget);
// 	mainWin.resize(640,480);
// 	mainWin.show();
// 
// 	return app.exec();

// 	Visualizer_ visualizer = Visualizer::getInstance();
// 	visualizer->setHumanMesh(human->getOriginalMesh());
// 	visualizer->setGarmentMesh(garment->getOriginalMesh());
// 
// 	human->addGarmentSimulationCallBack(visualizer);	
// 	human->dress(garment);
// 
// 	visualizer->show(argc, argv);
// 	/* Output */
// 	bool suc = OpenMesh::IO::write_mesh(*(garment->getOriginalMesh()), config->clothOutPath+config->clothInFileName);
// 	if(suc){
// 		PRINTLN("write succsss!");
// 	}
// 	else{
// 		PRINTLN("write fail!");
// 	}
// 	

// 	for(auto it = hRawmesh->vertices_begin(); it != hRawmesh->vertices_end(); it++){
// 		hRawmesh->point(*it) /= 10.0;
// 	}
// 	OpenMesh::IO::write_mesh(*(hRawmesh), config->humanInPath+hRawmesh->getName()+"_decimetre.obj");
// 	for(auto it = gRawmesh->vertices_begin(); it != gRawmesh->vertices_end(); it++){
// 		gRawmesh->point(*it) /= 10.0;
// 	}
// 	OpenMesh::IO::write_mesh(*(gRawmesh), config->clothInPath+gRawmesh->getName()+"_decimetre.obj");
 	getchar();

	return 0;
}