#include "gl/glew.h"
#include "VisibleMesh.h"
#include "Mesh.h"
#include <QImage>
#include <QGLWidget>
#include "MeshMaterial.h"
#define BUFFER_OFFSET(bytes) ((GLubyte*)NULL + bytes)

VisibleMesh::VisibleMesh(void)
{	
}

VisibleMesh::VisibleMesh( Mesh_ mesh )
{
	initWithMesh(mesh);	
}


VisibleMesh::~VisibleMesh(void)
{
}

void VisibleMesh::initWithMesh( Mesh_ mesh )
{
	mMesh = mesh;
	mVerCount = mesh->n_vertices();
	mFaceCount = mesh->n_faces();
	mTextureId = 0;

	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);
	
	glGenBuffers(NUMS_OF_VBO, mVBOs);

	/* Vertex */
	glBindBuffer(GL_ARRAY_BUFFER, mVBOs[VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(double) * mVerCount, mesh->points(), GL_DYNAMIC_DRAW);	
	glVertexPointer(3, GL_DOUBLE, 0, BUFFER_OFFSET(0));
	glEnableClientState(GL_VERTEX_ARRAY);

	/* Normal */
	glBindBuffer(GL_ARRAY_BUFFER, mVBOs[NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(double) * mVerCount, mesh->vertex_normals(), GL_DYNAMIC_DRAW);	
	glNormalPointer(GL_DOUBLE, 0, BUFFER_OFFSET(0));
	glEnableClientState(GL_NORMAL_ARRAY);

	/* Texture Coord */
	if(mMesh->has_vertex_texcoords2D() && loadTexture()){
		int vi = 0;
		Mesh::TexCoord2D* texcoords = new Mesh::TexCoord2D[mesh->n_vertices()];
		for(auto it = mesh->vertices_begin(); it != mesh->vertices_end(); it++){
			Mesh::TexCoord2D t = mesh->texcoord2D(*it);
			texcoords[vi++] = t;
		}		
		glBindBuffer(GL_ARRAY_BUFFER, mVBOs[TEXCOORD]);
		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(double) * mVerCount, texcoords, GL_STATIC_DRAW);		
		glTexCoordPointer(2, GL_DOUBLE, 0, BUFFER_OFFSET(0));
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		delete [] texcoords;
	}
	else{
		glDeleteBuffers(1, &mVBOs[TEXCOORD]);
	}
	/* Color */
	if(mMesh->has_vertex_colors()){
		glBindBuffer(GL_ARRAY_BUFFER, mVBOs[COLOR]);
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(unsigned char) * mVerCount, mMesh->vertex_colors(), GL_STATIC_DRAW);
		glColorPointer(3, GL_UNSIGNED_BYTE, 0, BUFFER_OFFSET(0));
		glEnableClientState(GL_COLOR_ARRAY);
	}
	else{
		glDeleteBuffers(1, &mVBOs[COLOR]);
	}
	/* Index */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBOs[ELEMENTS]);
	int vi = 0;
	GLuint* indices_array  = new GLuint[mesh->n_faces() * 3];
	std::for_each(mesh->faces_begin(), mesh->faces_end(), [&](const Mesh::FaceHandle f_it){
		int t = 0;
		for (auto face_v_it = mesh->cfv_iter(f_it); face_v_it.is_valid(); ++face_v_it){
			t++;
			indices_array[vi++] = face_v_it->idx();
		}
		assert(t == 3);//not a triangle face
	});	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * mFaceCount * sizeof(GLuint), indices_array, GL_STATIC_DRAW);

	glBindVertexArray(0);
	delete [] indices_array;
}

void VisibleMesh::draw()
{
	glBindVertexArray(mVAO);
	if(mTextureId != 0){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mTextureId);
	}	
	glDrawElements(GL_TRIANGLES, mMesh->n_faces() * 3, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
	if(mTextureId != 0){
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glBindVertexArray(0);
}

void VisibleMesh::updateVertices( std::vector<Vec3d>& newVers )
{
	if(newVers.size() != mVerCount){
		PRINT_ERROR("In VisibleMesh::updateVertices. Invalid newVers.");
		return;
	}
	Mesh tmp(*mMesh);
	for(size_t i = 0; i < mVerCount; i++){
		tmp.set_point(Mesh::VertexHandle(i), newVers[i]);
	}
	tmp.request_face_normals();
	tmp.update_normals();
	tmp.release_face_normals();
	GLsizeiptr size = 3 * sizeof(double) * mVerCount;
	glBindBuffer(GL_ARRAY_BUFFER, mVBOs[VERTEX]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, tmp.points());
	glBindBuffer(GL_ARRAY_BUFFER, mVBOs[NORMAL]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, tmp.vertex_normals());
}


void VisibleMesh::update()
{
	mMesh->request_face_normals();
	mMesh->update_normals();
	mMesh->release_face_normals();
	GLsizeiptr size = 3 * sizeof(double) * mVerCount;
	glBindBuffer(GL_ARRAY_BUFFER, mVBOs[VERTEX]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, mMesh->points());
	glBindBuffer(GL_ARRAY_BUFFER, mVBOs[NORMAL]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, mMesh->vertex_normals());
}

bool VisibleMesh::loadTexture()
{		
	QImage buf;
	MeshMaterial_ material = mMesh->getMeshMaterial();
	std::string file = material->map_Ka;
	bool suc = false;
	if(file != ""){
		suc = buf.load(material->map_Ka.c_str());
	}	
	if(!suc)
		return false;
	glGenTextures(1, &mTextureId);
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	QImage tex = QGLWidget::convertToGLFormat(buf);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}
