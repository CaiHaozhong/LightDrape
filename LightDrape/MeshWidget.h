#pragma once
#include "GarmentSimulationCallBack.h"
#include "Common.h"
#include "qglviewerwidget.h"
class Mesh;
class Human;
class Garment;
class MeshFramePool;
class VisibleMesh;
class ShaderProgram;
S_PTR(Mesh);
S_PTR(Human);
S_PTR(Garment);
S_PTR(MeshFramePool);
S_PTR(ShaderProgram);
S_PTR(VisibleMesh);
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
	/* 开始模拟 */
	void onSimulateBegin();

	/* 每计算出一帧时调用，这个函数不应该执行耗时的操作，不然会拖慢模拟过程 */
	void onFrame(MeshFrame_ frame);

	/* 结束模拟 */
	void onSimulateEnd(MeshFramePool_ mMeshFramePool);
private:
	std::weak_ptr<Human> mHuman;
	Garment_ mGarment;
	VisibleMesh_ mVisibleHuman, mVisibleGarment;
	std::vector<VisibleMesh_> mVisbleMeshes;
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
	GLuint mTexVBO;
	MeshFramePool_ mMeshFramePool;
	int mTimerID;
	int mCurFrameIndex;

	ShaderProgram_ mShaderProgram;
	
};
S_PTR(MeshWidget);