#include "Visualizer.h"
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/freeglut.h>


Visualizer::Visualizer(void)
{
	mStartTime = 0;
	mWidth = 640;
	mHeight = 480;
}


Visualizer::~Visualizer(void)
{
}

void Visualizer::onSimulateBegin()
{

}

void Visualizer::onFrame( MeshFrame_ frame )
{

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
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(mWidth, mHeight);
	glutCreateWindow("GLUT Cloth Demo [Explicit Euler Integration]");

 	glutDisplayFunc();
// 	glutReshapeFunc(OnReshape);
// 
// 	glutMouseFunc(OnMouseDown);
// 	glutMotionFunc(OnMouseMove);
// 
// 	glutCloseFunc(OnShutdown);

//	glutTimerFunc(4, tick, 1);

	glewInit();

	initGL();

	glutMainLoop();
}

void Visualizer::display()
{

}
