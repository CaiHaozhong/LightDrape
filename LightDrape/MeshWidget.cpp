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
using namespace std;
#define BUFFER_OFFSET(bytes) ((GLubyte*)NULL + bytes)

MeshWidget::MeshWidget(void)
{
	mMeshFramePool = nullptr;
	mCurFrameIndex = 0;
	mShaderProgram = smartNew(ShaderProgram);
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
 	humanSp->dress(mGarment);
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
		Mesh_ garMesh = mGarment->getOriginalMesh();
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
