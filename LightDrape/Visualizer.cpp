#include "Visualizer.h"
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/freeglut.h>
#include "Mesh.h"
#include "MeshFramePool.h"

Visualizer_ Visualizer::mInstance = nullptr;
Visualizer::Visualizer(void)
{
	mStartTime = 0;
	mWidth = 640;
	mHeight = 480;
	mRx = 15;
	mRy = 0;
	mDist = -23;
	mUp = Vec3d(0, 1, 0);	
	mHumanMesh = nullptr;
	mGarmentMesh = nullptr;
	mFrames = smartNew(SafeQueue);
	mCurFrame = nullptr;
}


Visualizer::~Visualizer(void)
{
}

void Visualizer::onSimulateBegin()
{
	std::cout << "Visualizer::onSimulateBegin()\n";
}

void Visualizer::onFrame( MeshFrame_ frame )
{
	std::cout << "f ";
	mFrames->push(frame);
}

void Visualizer::onSimulateEnd( MeshFramePool_ mMeshFramePool )
{

}

void Visualizer::initGL()
{
	mStartTime = (float)glutGet(GLUT_ELAPSED_TIME);
	mCurrentTime = mStartTime;

// 	// get ticks per second
// 	QueryPerformanceFrequency(&frequency);
// 
// 	// start timer
// 	QueryPerformanceCounter(&t1);



	glEnable(GL_DEPTH_TEST);

	glPointSize(5);

	wglSwapIntervalEXT(0);

	setLight();
}

void Visualizer::setLight()
{
	glClearColor(0,0,0,0);	

	GLfloat mat_a[] = {0.1f, 0.1, 0.1, 1.0};
	GLfloat mat_d[] = {0.7f, 0.7, 0.5, 1.0};
	GLfloat mat_s[] = {1.0f, 1.0, 1.0, 1.0};
	GLfloat shine[] = {50.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_a);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_d);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_s);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);

	GLfloat pos1[] = { 0,  20, 6, 0.0};
	GLfloat pos2[] = { 8,  0, 6, 0.0};
	GLfloat pos3[] = { -8,  0,  6,  0.0};
	GLfloat col1[] = { 0.7,  0.7,  0.8,  1.0};
	GLfloat col2[] = { 0.8,  0.7,  0.7,  1.0};
	GLfloat col3[] = { 1.0,  1.0,  1.0,  1.0};
	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);    
	glLightfv(GL_LIGHT0,GL_POSITION, pos1);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,  col1);
	glLightfv(GL_LIGHT0,GL_SPECULAR, col1);

	glEnable(GL_LIGHT1);  
	glLightfv(GL_LIGHT1,GL_POSITION, pos2);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,  col2);
	glLightfv(GL_LIGHT1,GL_SPECULAR, col2);

	glEnable(GL_LIGHT2);  
	glLightfv(GL_LIGHT2,GL_POSITION, pos3);
	glLightfv(GL_LIGHT2,GL_DIFFUSE,  col3);
	glLightfv(GL_LIGHT2,GL_SPECULAR, col3);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, col3); //设定全局环境光
	glShadeModel(GL_SMOOTH);
}

void Visualizer::show( int argc, char** argv )
{
	mInstance = shared_from_this();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(mWidth, mHeight);
	glutCreateWindow("GLUT Cloth Demo [Explicit Euler Integration]");

 	glutDisplayFunc(OnRender);

 	glutReshapeFunc(OnReshape);
// 
// 	glutMouseFunc(OnMouseDown);
// 	glutMotionFunc(OnMouseMove);
// 
// 	glutCloseFunc(OnShutdown);

	glutTimerFunc(4, tick, 1);

	glewInit();

	initGL();

	glutMainLoop();
}

void Visualizer::OnReshape( int nw, int nh )
{
	glViewport(0, 0, nw, nh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)nw / (GLfloat)nh, 1.f, 100.0f);

	glGetIntegerv(GL_VIEWPORT, mInstance->mViewport);
	glGetDoublev(GL_PROJECTION_MATRIX, mInstance->mProjMatrix);

	glMatrixMode(GL_MODELVIEW);
}

void Visualizer::OnRender()
{	
//	sprintf_s(info, "FPS: %3.2f, Frame time (GLUT): %3.4f msecs, Frame time (QP): %3.3f", fps, frameTime, frameTimeQP);
	glutSetWindowTitle("FPS: %3.2f, Frame time (GLUT): %3.4f msecs, Frame time (QP): %3.3f");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0, -5, mInstance->mDist);
	glRotatef(mInstance->mRx, 1, 0, 0);
	glRotatef(mInstance->mRy, 0, 1, 0);

	glGetDoublev(GL_MODELVIEW_MATRIX, mInstance->mMVMatrix);
	mInstance->mViewDir.values_[0] = (float) - mInstance->mMVMatrix[2];
	mInstance->mViewDir.values_[1] = (float) - mInstance->mMVMatrix[6];
	mInstance->mViewDir.values_[2] = (float) - mInstance->mMVMatrix[10];
	mInstance->mRight = mInstance->mViewDir % mInstance->mUp;
	
	//draw grid
 	mInstance->DrawGrid();
// 
 	mInstance->drawMesh();

	glutSwapBuffers();
}

void Visualizer::DrawGrid()
{
	glBegin(GL_LINES);
	glColor3f(0.5f, 0.5f, 0.5f);
	for(int i = -GRID_SIZE; i <= GRID_SIZE; i++)
	{
		glVertex3f((float)i,5,(float)-GRID_SIZE);
		glVertex3f((float)i,5,(float)GRID_SIZE);

		glVertex3f((float)-GRID_SIZE,5,(float)i);
		glVertex3f((float)GRID_SIZE,5,(float)i);
	}
	glEnd();
}

void Visualizer::drawMesh()
{
// 	/* Draw Human */
// 	if(mHumanMesh != nullptr){
// 		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
// 		glBegin(GL_TRIANGLES);
// 		for(auto it = mHumanMesh->faces_begin(); it != mHumanMesh->faces_end(); it++){
// 			for(auto fv_it = mHumanMesh->fv_begin(*it); fv_it.is_valid(); fv_it++){
// 				size_t index = fv_it->idx();
// 				auto& p = mHumanMesh->point(*fv_it).values_;
// 				const auto& n = mHumanMesh->normal(*fv_it).values_;
// 				glVertex3dv(p);
// 				glNormal3dv(n);
// 			}
// 		}
// 		glEnd();
// 	}


	MeshFrame_ curFrame = mFrames->pop();	
	if(curFrame == nullptr)
		std::cout << "null ";
	if(curFrame != nullptr){
		std::vector<Vec3d>& frameVers = curFrame->getVertices();
		size_t cur = 0;
		for(auto it = mGarmentMesh->vertices_begin(); it != mGarmentMesh->vertices_end(); it++){
			Vec3d& p = mGarmentMesh->point(*it);
			p = frameVers[cur++];
		}
		// we need face normals to update the vertex normals
		mGarmentMesh->request_face_normals();
		// let the mesh update the normals
		mGarmentMesh->update_normals();
		// dispose the face normals, as we don't need them anymore
		mGarmentMesh->release_face_normals();
		glColor3f(0.8f, 0.8f, 0.8f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(GL_TRIANGLES);
		for(auto it = mGarmentMesh->faces_begin(); it != mGarmentMesh->faces_end(); it++){
			for(auto fv_it = mGarmentMesh->fv_begin(*it); fv_it.is_valid(); fv_it++){
				size_t index = fv_it->idx();
				auto& p = frameVers[index].values_;
				const auto& n = mGarmentMesh->normal(*fv_it).values_;
				glVertex3dv(p);
				glNormal3dv(n);
			}
		}
		glEnd();
	}
}

void Visualizer::setHumanMesh( Mesh_ mesh )
{
	mHumanMesh = mesh;
}

void Visualizer::setGarmentMesh( Mesh_ mesh )
{
	mGarmentMesh = mesh;
}

void Visualizer::tick( int val )
{
	glutPostRedisplay();
	glutTimerFunc(4, tick, 1);
}

MeshFrame_ SafeQueue::pop()
{
	MeshFrame_ ret = nullptr;
	m.lock();
	if(mFrames.size() > 0){
		ret = mFrames.front();
		mFrames.pop();
	}
	m.unlock();
	return ret;
}

void SafeQueue::push( MeshFrame_ frame )
{
	m.lock();
	mFrames.push(frame);
	m.unlock();
}
