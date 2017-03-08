#include "Visualizer.h"
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/freeglut.h>
#include "Mesh.h"
#include "MeshFramePool.h"
#include "GLError.h"

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
	mMeshFramePool = nullptr;
	mCurFrameIndex = 0;

	oldX = 0;
	oldY = 0;
	state = 1;
	selected_index = -1;

	mShowHuman = glGenLists(1);
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
}

void Visualizer::onSimulateEnd( MeshFramePool_ meshFramePool )
{
	mMeshFramePool = meshFramePool;
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

// 
// 
// 	glEnable(GL_DEPTH_TEST);
// 
// 	glPointSize(5);
// 
// 	wglSwapIntervalEXT(0);
// 
// 	setLight();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(2.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST); //用来开启深度缓冲区的功能，启动后OPengl就可以跟踪Z轴上的像素，那么它只有在前面没有东西的情况下才会绘制这个像素，在绘制3d时，最好启用，视觉效果会比较真实
	// ------------------- Lighting  
	glEnable(GL_LIGHTING); // 如果enbale那么就使用当前的光照参数去推导顶点的颜色
	glEnable(GL_LIGHT0); //第一个光源，而GL_LIGHT1表示第二个光源

	prepare(*mHumanMesh, mHumanVBO);
//	prepare(*mGarmentMesh, mGarmentVBO);
	
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
	if(mHumanMesh == nullptr || mGarmentMesh == nullptr){
		PRINTLN("In Visualizer::show, mHumanMesh == nullptr || mGarmentMesh == nullptr");
		return ;
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(mWidth, mHeight);
	glutCreateWindow("GLUT Cloth Demo [Explicit Euler Integration]");

 	glutDisplayFunc(OnRender);

 	glutReshapeFunc(OnReshape);
// 
 	glutMouseFunc(OnMouseDown);
 	glutMotionFunc(OnMouseMove);
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
	/* Draw Human */
	if(mHumanMesh != nullptr){
// 		glCallList(mShowHuman);
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

		glBindBuffer(GL_ARRAY_BUFFER,mHumanVBO[VBO_VERTEX]);
		glVertexPointer(3,GL_FLOAT,0,0);

		/* 指定法向量的offset */
		glBindBuffer(GL_ARRAY_BUFFER, mHumanVBO[VBO_NORMAL]);
		glNormalPointer(GL_FLOAT,0, 0);

		/* 绘制 */
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mHumanVBO[VBO_INDEX]);
		glDrawElements(GL_TRIANGLES, mHumanMesh->n_faces() * 3, GL_UNSIGNED_INT, 0);
	}
// 
// 	if(mCurFrame != nullptr){
// 		std::vector<Vec3d>& frameVers = mCurFrame->getVertices();
// 		size_t cur = 0;
// 		for(auto it = mGarmentMesh->vertices_begin(); it != mGarmentMesh->vertices_end(); it++){
// 			Vec3d& p = mGarmentMesh->point(*it);
// 			p = frameVers[cur++];
// 		}
// 		// we need face normals to update the vertex normals
// 		mGarmentMesh->request_face_normals();
// 		// let the mesh update the normals
// 		mGarmentMesh->update_normals();
// 		// dispose the face normals, as we don't need them anymore
// 		mGarmentMesh->release_face_normals();
// 		glColor3f(0.8f, 0.8f, 0.8f);
// 		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
// 		glBegin(GL_TRIANGLES);
// 		for(auto it = mGarmentMesh->faces_begin(); it != mGarmentMesh->faces_end(); it++){
// 			for(auto fv_it = mGarmentMesh->fv_begin(*it); fv_it.is_valid(); fv_it++){
// 				size_t index = fv_it->idx();
// 				auto& p = frameVers[index].values_;
// 				const auto& n = mGarmentMesh->normal(*fv_it).values_;
// 				glVertex3dv(p);
// 				glNormal3dv(n);
// 			}
// 		}
// 		glEnd();
// 	}
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
	MeshFramePool_ framePool = mInstance->mMeshFramePool;
	if(framePool != nullptr){
		size_t frameCount = framePool->getFrameCount();
		mInstance->mCurFrame = framePool->getFrame((mInstance->mCurFrameIndex++)%frameCount);
		glutPostRedisplay();
	}
	glutTimerFunc(4, tick, 1);
}

void Visualizer::OnMouseDown( int button, int s, int x, int y )
{
	if (s == GLUT_DOWN)
	{
		mInstance->oldX = x;
		mInstance->oldY = y;
		int window_y = (mInstance->mHeight - y);
		float norm_y = float(window_y)/float(mInstance->mHeight/2.0);
		int window_x = x ;
		float norm_x = float(window_x)/float(mInstance->mWidth/2.0);
	}

	if(button == GLUT_MIDDLE_BUTTON)
		mInstance->state = 0;
	else
		mInstance->state = 1;

	if(s==GLUT_UP) {
		mInstance->selected_index= -1;
		glutSetCursor(GLUT_CURSOR_INHERIT);
	}
}

void Visualizer::OnMouseMove( int x, int y )
{
	if(mInstance->selected_index == -1) {
		if (mInstance->state == 0)
			mInstance->mDist *= (1 + (y - mInstance->oldY)/60.0f);
		else
		{
			mInstance->mRy += (x - mInstance->oldX)/5.0f;
			mInstance->mRx += (y - mInstance->oldY)/5.0f;
		}
	} else {
		float delta = 1500/abs(mInstance->mDist);
		float valX = (x - mInstance->oldX)/delta;
		float valY = (mInstance->oldY - y)/delta;
		if(abs(valX)>abs(valY))
			glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
		else
			glutSetCursor(GLUT_CURSOR_UP_DOWN);

// 		V[selected_index] = glm::vec3(0);
// 		X[selected_index].x += Right[0]*valX ;
// 		float newValue = X[selected_index].y+Up[1]*valY;
// 		if(newValue>0)
// 			X[selected_index].y = newValue;
// 		X[selected_index].z += Right[2]*valX + Up[2]*valY;
// 
// 		V[selected_index].x = 0;
// 		V[selected_index].y = 0;
// 		V[selected_index].z = 0;
	}
	mInstance->oldX = x;
	mInstance->oldY = y;

	glutPostRedisplay();
}

Visualizer_ Visualizer::getInstance()
{
	if(mInstance == nullptr){
		mInstance = std::shared_ptr<Visualizer>(new Visualizer);
	}
	return mInstance;
}

void Visualizer::prepare( Mesh& _mesh, unsigned int* _vbo)
{
	int vi = 0;
	GLuint* indices_array  = new GLuint[_mesh.n_faces() * 3];
	std::for_each(_mesh.faces_begin(), _mesh.faces_end(), [&](const Mesh::FaceHandle f_it){
		int t = 0;
		for (auto face_v_it = _mesh.cfv_iter(f_it); face_v_it.is_valid(); ++face_v_it){
			t++;
			indices_array[vi++] = face_v_it->idx();
		}
		assert(t == 3); //not a triangle face
	});		

	check_gl_error();
	/* 生成三个VBO对象 */
	glGenBuffers(VBO_SIZE,_vbo);
	check_gl_error();
	/* 将顶点数据拷贝到顶点VBO中 */
	glBindBuffer(GL_ARRAY_BUFFER,_vbo[VBO_VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * _mesh.n_vertices() * 1 , _mesh.points(), GL_DYNAMIC_DRAW);
	glEnableClientState(GL_VERTEX_ARRAY);		
	//	glVertexPointer(3,GL_FLOAT,0,0);
	check_gl_error();
	/* 将法线数据拷贝到法线VBO中 */
	glBindBuffer(GL_ARRAY_BUFFER, _vbo[VBO_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * _mesh.n_vertices() * 1 , _mesh.vertex_normals(), GL_STATIC_DRAW);
	glEnableClientState(GL_NORMAL_ARRAY);
	//	glNormalPointer(GL_FLOAT,0, 0);
	check_gl_error();
	/* 将索引数据拷贝到索引的VBO中，而索引如何与顶点和法线关联起来，
	靠的是glVertexPointer和glNormalPointer，在绘制的时候指定就好 */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo[VBO_INDEX]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLuint) * 3 * _mesh.n_faces(),indices_array,GL_STATIC_DRAW);
	delete [] indices_array;
	check_gl_error();
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
