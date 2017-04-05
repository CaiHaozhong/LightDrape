#include <OpenMesh/Core/IO/MeshIO.hh>
#include "MeshWidget.h"
#include "Mesh.h"
#include <string>
#include "GLError.h"
#include "Human.h"
#include "Garment.h"
#include "MeshFramePool.h"
#include "FrameToOBJFileWriter.h"
#include <QImage>
#include "MeshMaterial.h"
#include "shaderprogram.h"
#include "VisibleMesh.h"
#include "Mesher.h"
#include "Config.h"
#include "GarmentPenetrationResolver.h"
#include "MeshWriter.h"
using namespace std;
#define BUFFER_OFFSET(bytes) ((GLubyte*)NULL + bytes)

MeshWidget::MeshWidget(void)
{
	mMeshFramePool = nullptr;
	mCurFrameIndex = 0;	
	mFrameCount = 0;
}


MeshWidget::~MeshWidget(void)
{
}

void MeshWidget::draw_scene( const std::string& _draw_mode )
{
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	for(size_t i = 0; i < mVisbleMeshes.size(); i++){
		mVisbleMeshes[i]->draw();
	}
	return;
}

void MeshWidget::initGlew()
{
	makeCurrent();
	GLenum err = glewInit();
	if (GLEW_OK != err){
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
}

void MeshWidget::onEndInitializeGL()
{
	initGlew();
	sendDataToGPU();
	Human_ humanSp = mHuman.lock();
	Vec3d center = (mGarment->getMax() + mGarment->getMin()) * 0.5;
	double radius = (mGarment->getMax() - mGarment->getMin()).length() * 0.5;
	set_scene_pos(center, radius);
 	humanSp->addGarmentSimulationCallBack(std::shared_ptr<MeshWidget>(this));
	//humanSp->addMeshSegmentListener(std::shared_ptr<MeshWidget>(this));
 	humanSp->dress(mGarment);
 	Config_ config = Config::getInstance();
 	mVisibleGarment->update();
 	update();
 	/* Output */
// 	MeshWriter::writeMesh(mGarment->getOriginalMesh(), 
// 		config->clothOutPath,
// 		humanSp->getName() + "_"
// 		+ mGarment->getName() + ".obj");
	MeshWriter::writeMesh(mGarment->getOriginalMesh(), 
		config->clothOutPath,		
		mGarment->getName() + "_rp.obj");

// 	OpenMesh::IO::Options opt;
// 	opt += OpenMesh::IO::Options::VertexTexCoord;
// 	opt += OpenMesh::IO::Options::FaceTexCoord;
//   	bool suc = OpenMesh::IO::write_mesh(*(mGarment->getOriginalMesh()), config->clothOutPath
// 		+ humanSp->getName() + "_"
// 		+ mGarment->getName() + ".obj", opt);
// 	if(suc){
// 		PRINTLN("write succsss!");
// 	}
// 	else{
// 		PRINTLN("write fail!");
// 	}
 	mTimerID = this->startTimer(4);
}

void MeshWidget::setHuman( Human_ human )
{
	mHuman = human;	
}

void MeshWidget::setGarment( Garment_ garment )
{
	mGarment = garment;	
}

void MeshWidget::onSimulateBegin()
{
}

void MeshWidget::onFrame( MeshFrame_ frame )
{
	PRINT("h ");
	mFrameCount += 1;
	Config_ config = Config::getInstance();
	std::vector<int>& resultFrames = config->resultFrames;
	for(size_t f = 0; f < resultFrames.size(); f++){
		if(mFrameCount == resultFrames[f]){
			Mesh_ ret = std::make_shared<Mesh>(*(mGarment->getOriginalMesh()));
			std::vector<Vec3d>& newVers = frame->getVertices();
			for(size_t i = 0; i < newVers.size(); i++){
				ret->set_point(Mesh::VertexHandle(i), newVers[i]);
			}
			ret->request_face_normals();
			ret->update_normals();
			ret->release_face_normals();
			char buf[30];
			_itoa(mFrameCount, buf, 10);
			GarmentPenetrationResolver_ penetrationResolver = smartNew(GarmentPenetrationResolver);
			penetrationResolver->setGarment(ret);
			penetrationResolver->setHuman(mHuman.lock()->getOriginalMesh());
			bool isSuc = penetrationResolver->resolve();
			if(isSuc){
				PRINTLN("Resolve Penetration successfully.");
			}
			else{
				PRINTLN("Resolve Penetration fail.");
			}
			MeshWriter::writeMesh(ret, config->clothOutPath,
				"physic_nopenetration_" +
				std::string(buf) + 
				"_" + mGarment->getName() + ".obj");
// 			OpenMesh::IO::Options opt;
// 			opt += OpenMesh::IO::Options::VertexTexCoord;
// 			OpenMesh::IO::write_mesh(*ret, config->clothOutPath + "physic_nopenetration_" +
// 				std::string(buf) + 
// 				"_" + mGarment->getName() + ".obj", opt);
		}
	}
}

void MeshWidget::onSimulateEnd( MeshFramePool_ meshFramePool )
{
	this->mMeshFramePool = meshFramePool;	
}

void MeshWidget::timerEvent( QTimerEvent *event )
{
	if(mMeshFramePool != nullptr){
		size_t frameSize = mMeshFramePool->getFrameCount();
		MeshFrame_ curFrame = mMeshFramePool->getFrame((mCurFrameIndex++)%frameSize);		
		std::vector<Vec3d>& vers = curFrame->getVertices();
		mVisibleGarment->updateVertices(vers);
		update();
	}
}

void MeshWidget::paintGL()
{
	QGLViewerWidget::paintGL();
}

void MeshWidget::sendDataToGPU()
{
	mVisibleGarment = std::make_shared<VisibleMesh>(mGarment->getOriginalMesh());
	mVisibleHuman = std::make_shared<VisibleMesh>(mHuman.lock());
	mVisbleMeshes.push_back(mVisibleGarment);
	mVisbleMeshes.push_back(mVisibleHuman);
}

void MeshWidget::onEndCoarseSegment( Segment_ seg )
{
	std::vector< std::pair<int, Region_> >& regions = seg->getRegionsRaw();
	for(std::vector< std::pair<int, Region_> >::iterator it = regions.begin();
		it != regions.end(); it++){
			Region_ reg = it->second;
			Mesher mesher(reg);
			Mesh_ m = mesher.getMesh();
			m->request_vertex_normals();
			m->request_face_normals();
			m->update_normals();
			m->release_face_normals();
			m->request_vertex_colors();
			Vec3uc regColor = reg->getColor();
			for(auto v_it = m->vertices_begin(); v_it != m->vertices_end(); v_it++){
				m->set_color(*v_it, regColor);
			}
			mVisbleMeshes.push_back(std::make_shared<VisibleMesh>(m));
	}
}
