#include "MeshWidget.h"
#include "Mesh.h"
#include <string>
#include "GLError.h"
#include "Human.h"
#include "Garment.h"
#include "MeshFramePool.h"
#include "FrameToOBJFileWriter.h"
using namespace std;

MeshWidget::MeshWidget(void)
{
	mMeshFramePool = nullptr;
	mCurFrameIndex = 0;
}


MeshWidget::~MeshWidget(void)
{
}

void MeshWidget::draw_scene( const std::string& _draw_mode )
{
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);

/* --------------------------- Draw Human ----------------------------------*/
	/* 指定顶点数据的offset */
	glBindBuffer(GL_ARRAY_BUFFER,mHumanVBO[VBO_VERTEX]);
	glVertexPointer(3,GL_DOUBLE,0,0);

	/* 指定法向量的offset */
	glBindBuffer(GL_ARRAY_BUFFER, mHumanVBO[VBO_NORMAL]);
	glNormalPointer(GL_DOUBLE,0, 0);

	/* 绘制 */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mHumanVBO[VBO_INDEX]);
	glDrawElements(GL_TRIANGLES, mHuman.lock()->getOriginalMesh()->n_faces() * 3, GL_UNSIGNED_INT, 0);

/* --------------------------- Draw Garment ----------------------------------*/
	/* 指定顶点数据的offset */
	glBindBuffer(GL_ARRAY_BUFFER,mGarmentVBO[VBO_VERTEX]);
	glVertexPointer(3,GL_DOUBLE,0,0);

	/* 指定法向量的offset */
	glBindBuffer(GL_ARRAY_BUFFER, mGarmentVBO[VBO_NORMAL]);
	glNormalPointer(GL_DOUBLE,0, 0);

	

	/* 绘制 */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGarmentVBO[VBO_INDEX]);
	glDrawElements(GL_TRIANGLES, mGarment->getOriginalMesh()->n_faces() * 3, GL_UNSIGNED_INT, 0);
}

void MeshWidget::prepare( Mesh& _mesh , GLuint* _vbo)
{	
	int vi = 0;
	GLuint* indices_array  = new GLuint[_mesh.n_faces() * 3];
	std::for_each(_mesh.faces_begin(), _mesh.faces_end(), [&](const Mesh::FaceHandle f_it){
		int t = 0;
		for (auto face_v_it = _mesh.cfv_iter(f_it); face_v_it.is_valid(); ++face_v_it){
			t++;
			indices_array[vi++] = face_v_it->idx();
		}
		assert(t == 3, "not triangle face");
	});		

	check_gl_error();
	/* 生成三个VBO对象 */
	glGenBuffers(VBO_SIZE,_vbo);
	check_gl_error();
	/* 将顶点数据拷贝到顶点VBO中 */
	glBindBuffer(GL_ARRAY_BUFFER,_vbo[VBO_VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(double) * 3 * _mesh.n_vertices() * 1 , _mesh.points(), GL_DYNAMIC_DRAW);
	glEnableClientState(GL_VERTEX_ARRAY);		
//	glVertexPointer(3,GL_FLOAT,0,0);
	check_gl_error();
	/* 将法线数据拷贝到法线VBO中 */
	glBindBuffer(GL_ARRAY_BUFFER, _vbo[VBO_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(double) * 3 * _mesh.n_vertices() * 1 , _mesh.vertex_normals(), GL_DYNAMIC_DRAW);
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
	Human_ humanSp = mHuman.lock();
	Vec3d center = (humanSp->getMax() + humanSp->getMin()) * 0.5;
	double radius = (humanSp->getMax() - humanSp->getMin()).length() * 0.5;
	set_scene_pos(center, radius);
	prepare(*(humanSp->getOriginalMesh()), mHumanVBO);	
	prepare(*(mGarment->getOriginalMesh()), mGarmentVBO);
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
		size_t verSize = vers.size();
		size_t i = 0;
		for(auto it = garMesh->vertices_begin(); it != garMesh->vertices_end(); it++){
			Vec3d& p = garMesh->point(*it);
			p = vers[i++];
		}
		garMesh->request_face_normals();
		garMesh->update_normals();
		garMesh->release_face_normals();
		/* 指定顶点数据的offset */
		glBindBuffer(GL_ARRAY_BUFFER,mGarmentVBO[VBO_VERTEX]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(double) * 3 * garMesh->n_vertices() * 1 , garMesh->points(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,mGarmentVBO[VBO_NORMAL]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(double) * 3 * garMesh->n_vertices() * 1 , garMesh->vertex_normals(), GL_DYNAMIC_DRAW);
		update();
	}
}
