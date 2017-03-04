#pragma once
#include "GarmentSimulationCallBack.h"
#include "Common.h"
#include "qglviewerwidget.h"
class Mesh;
class Human;
class Garment;
class MeshFramePool;
class ShaderProgram;
S_PTR(Mesh);
S_PTR(Human);
S_PTR(Garment);
S_PTR(MeshFramePool);
S_PTR(ShaderProgram);
class MeshWidget :
	public QGLViewerWidget, public GarmentSimulationCallBack, public std::enable_shared_from_this<MeshWidget>
{
public:
	MeshWidget(void);
	~MeshWidget(void);

	void setHuman(Human_ human);

	void setGarment(Garment_ garment);

protected:
	void paintGL();

	void onEndInitializeGL();

private:
	void draw_scene(const std::string& _draw_mode);

	void sendDataToGPU();

	void prepare(Mesh& mesh, GLuint* _vbo);

	void prepareGLSL();

	void initGlew();

	/* Override */
	void timerEvent( QTimerEvent *event );
public:
	/* ��ʼģ�� */
	void onSimulateBegin();

	/* ÿ�����һ֡ʱ���ã����������Ӧ��ִ�к�ʱ�Ĳ�������Ȼ������ģ����� */
	void onFrame(MeshFrame_ frame);

	/* ����ģ�� */
	void onSimulateEnd(MeshFramePool_ mMeshFramePool);
private:
	std::weak_ptr<Human> mHuman;
	Garment_ mGarment;
	enum{
		VBO_VERTEX,
		VBO_NORMAL,
		VBO_TEXCOORD,
		VBO_INDEX,
		VBO_SIZE
	};
	GLuint mHumanVBO[VBO_SIZE];
	GLuint mGarmentVBO[VBO_SIZE];
	GLuint mTexture;	
	GLuint mVAO;
	GLuint mVBO;
	GLuint mEBO;
	MeshFramePool_ mMeshFramePool;
	int mTimerID;
	int mCurFrameIndex;

	ShaderProgram_ mShaderProgram;
	
};
S_PTR(MeshWidget);