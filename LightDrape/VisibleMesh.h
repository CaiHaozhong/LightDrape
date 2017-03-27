#pragma once
#include "Common.h"
#include <vector>
#include "Vec3d.h"
#include "gl/freeglut.h"
class Mesh;
S_PTR(Mesh);
class VisibleMesh
{
private:
	typedef enum{
		VERTEX,
		NORMAL,
		TEXCOORD,
		COLOR,
		ELEMENTS,
		NUMS_OF_VBO
	} MESH_VBO;
	Mesh_ mMesh;
	GLuint mVAO;
	GLuint mVBOs[NUMS_OF_VBO];
	GLuint mTextureId;
	size_t mVerCount;
	size_t mFaceCount;	
public:
	VisibleMesh(void);
	~VisibleMesh(void);
	VisibleMesh(Mesh_ mesh);
	void initWithMesh(Mesh_ mesh);
	void draw();
	void updateVertices(std::vector<Vec3d>& newVers);
	void update();

private:
	bool loadTexture();
};
S_PTR(VisibleMesh);