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
using namespace std;

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

/* --------------------------- Draw Human ----------------------------------*/
// 	/* 顶点数据 */
// 	glBindBuffer(GL_ARRAY_BUFFER,mHumanVBO[VBO_VERTEX]);
// 	glEnableClientState(GL_VERTEX_ARRAY);	
// 	glVertexPointer(3,GL_DOUBLE,0,0);
// 
// 	/* 法向量 */
// 	glBindBuffer(GL_ARRAY_BUFFER, mHumanVBO[VBO_NORMAL]);
// 	glEnableClientState(GL_NORMAL_ARRAY);
// 	glNormalPointer(GL_DOUBLE,0, 0);
// 
// 	/* 下标 */
// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mHumanVBO[VBO_INDEX]);
// 	glDrawElements(GL_TRIANGLES, mHuman.lock()->getOriginalMesh()->n_faces() * 3, GL_UNSIGNED_INT, 0);

/* --------------------------- Draw Garment ----------------------------------*/
	/* 顶点数据 */
	glBindBuffer(GL_ARRAY_BUFFER, mGarmentVBO[VBO_VERTEX]);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_DOUBLE,0,0);

	/* 法向量 */
	glBindBuffer(GL_ARRAY_BUFFER, mGarmentVBO[VBO_NORMAL]);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_DOUBLE,0, 0);

	glClientActiveTexture(GL_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, mGarmentVBO[VBO_TEXCOORD]);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_DOUBLE, 0, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	check_gl_error();
	/* 下标 */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGarmentVBO[VBO_INDEX]);
	glDrawElements(GL_TRIANGLES, mGarment->getOriginalMesh()->n_faces() * 3, GL_UNSIGNED_INT, 0);
	check_gl_error();
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
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

	/* Texture Coord */
	vi = 0;
	Mesh::TexCoord2D* texcoords = new Mesh::TexCoord2D[_mesh.n_vertices()];
	for(auto it = _mesh.vertices_begin(); it != _mesh.vertices_end(); it++){
		Mesh::TexCoord2D t = _mesh.texcoord2D(*it);
		texcoords[vi] = t;
	}
	check_gl_error();
	/* 生成三个VBO对象 */
	glGenBuffers(VBO_SIZE,_vbo);

	/* 将顶点数据拷贝到顶点VBO中 */
	glBindBuffer(GL_ARRAY_BUFFER,_vbo[VBO_VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(double) * 3 * _mesh.n_vertices(), _mesh.points(), GL_DYNAMIC_DRAW);

	/* 将法线数据拷贝到法线VBO中 */
	glBindBuffer(GL_ARRAY_BUFFER, _vbo[VBO_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(double) * 3 * _mesh.n_vertices(), _mesh.vertex_normals(), GL_DYNAMIC_DRAW);	

	glBindBuffer(GL_ARRAY_BUFFER, _vbo[VBO_TEXCOORD]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(double) * 2 * _mesh.n_vertices(), texcoords[0].values_, GL_STREAM_DRAW);
	check_gl_error();
	/* 将索引数据拷贝到索引的VBO中，而索引如何与顶点和法线关联起来，
	   靠的是glVertexPointer和glNormalPointer，在绘制的时候指定就好 */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo[VBO_INDEX]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLuint) * 3 * _mesh.n_faces(),indices_array,GL_STATIC_DRAW);



	/* Texture data */
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	QImage tex, buf;
	MeshMaterial_ material = _mesh.getMeshMaterial();
	bool suc = buf.load(material->map_Ka.c_str());
	tex = this->convertToGLFormat(buf);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glBindTexture(GL_TEXTURE_2D, 0);
	check_gl_error();

	delete [] indices_array;
	delete [] texcoords;

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
	prepareGLSL();
	sendDataToGPU();
//	Human_ humanSp = mHuman.lock();
	Vec3d center = (mGarment->getMax() + mGarment->getMin()) * 0.5;
	double radius = (mGarment->getMax() - mGarment->getMin()).length() * 0.5;
	set_scene_pos(center, radius);
//	prepare(*(humanSp->getOriginalMesh()), mHumanVBO);	
// 	prepare(*(mGarment->getOriginalMesh()), mGarmentVBO);
// 	humanSp->addGarmentSimulationCallBack(std::shared_ptr<MeshWidget>(this));
// 	humanSp->dress(mGarment);
// 	mTimerID = this->startTimer(4);
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
void MeshWidget::prepareGLSL()
{
	std::string ver_shader_file = "shader/vertex-shader.txt";
	std::string fra_shader_file = "shader/fragment-shader.txt";
// 
// 	/* Read Vertex Shader */
// 	std::ifstream f;
// 	f.open(ver_shader_file.c_str(), std::ios::in | std::ios::binary);
// 	if(!f.is_open()){
// 		throw std::runtime_error(std::string("Failed to open file: ") + ver_shader_file);
// 	}
// 
// 	//read whole file into stringstream buffer
// 	std::stringstream buffer;
// 	buffer << f.rdbuf();
// 
// 	std::string vertex_shader_code = buffer.str();
// 	f.close();
// /*	std::cout << vertex_shader_code << std::endl;*/
// 
// 	/* Read Fragment Shader */
// 	f.open(fra_shader_file.c_str(), std::ios::in | std::ios::binary);
// 	if(!f.is_open()){
// 		throw std::runtime_error(std::string("Failed to open file: ") + fra_shader_file);
// 	}
// 
// 	//read whole file into stringstream buffer
// 	std::stringstream buffer2;
// 	buffer2 << f.rdbuf();
// 	std::string fragment_shader_code = buffer2.str();
// 	f.close();
// /*	std::cout << fragment_shader_code << std::endl;*/
// 
// 
// 	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
// 	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
// 	const char* vsource = vertex_shader_code.c_str();
// 	const char* fsource = fragment_shader_code.c_str();
// 	glShaderSource(vs, 1, &vsource, NULL);  
// 	glShaderSource(fs, 1, &fsource, NULL);  
// 	glCompileShader(vs);  
// 	glCompileShader(fs);
// 	mPrograme = glCreateProgram(); 
// 	glAttachShader(mPrograme, vs);  
// 	glAttachShader(mPrograme, fs);  
// 
// 	glLinkProgram(mPrograme); 
//	glUseProgram(mPrograme);
	mShaderProgram->compileShaderFromFile(ver_shader_file.c_str(), GLSLShader::VERTEX);
	mShaderProgram->log();
	mShaderProgram->compileShaderFromFile(fra_shader_file.c_str(), GLSLShader::FRAGMENT);
	mShaderProgram->log();
	mShaderProgram->link();
	mShaderProgram->log();
	mShaderProgram->use();
	mShaderProgram->log();
}

void MeshWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mShaderProgram->setUniform("ModelViewMatrix", modelview_matrix_);
	mShaderProgram->setUniform("ProjectionMatrix", projection_matrix_);
	//draw_scene(std::string());
	glBindVertexArray(mVAO);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glDrawElements(GL_TRIANGLES, mGarment->getOriginalMesh()->n_faces() * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void MeshWidget::sendDataToGPU()
{
	Mesh_ mesh = mGarment->getOriginalMesh();
	size_t verSize = mesh->n_vertices();
	double * vertices = new double[verSize * 2 * 3];
	int cur = 0;
	for(auto it = mesh->vertices_begin(); it != mesh->vertices_end(); it++){
		const double* p = mesh->point(*it).values_;
		const double* n = mesh->normal(*it).values_;		
		for(int i = 0; i < 3; i++)
			vertices[cur++] = p[i];
		for(int i = 0; i < 3; i++)
			vertices[cur++] = n[i];
	}
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(double) * verSize, vertices, GL_DYNAMIC_DRAW);
// 	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(double) * verSize, mesh->points());
// 	glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(double) * verSize, 3 * sizeof(double) * verSize, mesh->vertex_normals());

	int vi = 0;
	GLuint* indices_array  = new GLuint[mesh->n_faces() * 3];
	std::for_each(mesh->faces_begin(), mesh->faces_end(), [&](const Mesh::FaceHandle f_it){
		int t = 0;
		for (auto face_v_it = mesh->cfv_iter(f_it); face_v_it.is_valid(); ++face_v_it){
			t++;
			indices_array[vi++] = face_v_it->idx();
		}
		assert(t == 3, "not triangle face");
	});	

	glGenBuffers(1, &mEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 3 * mesh->n_faces(), indices_array, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(GLdouble), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(GLdouble), (GLvoid*)(3 * sizeof(GLdouble)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	delete [] vertices;
}
