#pragma once
#include "Common.h"
#include "GarmentSimulationCallBack.h"
#include "Vec3d.h"
#include <queue>
#include <thread>
#include <mutex>
class MeshFrame;
S_PTR(MeshFrame);
class MeshFramePool;
S_PTR(MeshFramePool);
class Visualizer;
S_PTR(Visualizer);
class Mesh;
S_PTR(Mesh);
class SafeQueue{
private:
	std::queue<MeshFrame_> mFrames;
	std::mutex m;
public:
	MeshFrame_ pop();

	void push(MeshFrame_ frame);
};
S_PTR(SafeQueue);
class Visualizer : public GarmentSimulationCallBack, public std::enable_shared_from_this<Visualizer>
{
private:
	static Visualizer_ mInstance;

	double mStartTime;
	double mCurrentTime;
	double mFPS;

	double mWidth, mHeight;
	
	float mRx, mRy;
	float mDist;
	static const int GRID_SIZE = 10;

	int oldX, oldY;
	int state;
	int selected_index;

	int mViewport[4];
	double mMVMatrix[16];
	double mProjMatrix[16];

	Vec3d mUp/* = Vec3d(0, 1, 0)*/, mRight, mViewDir;

	Mesh_ mHumanMesh;
	Mesh_ mGarmentMesh;

	size_t mShowHuman, mShowGarment;

	SafeQueue_ mFrames;

	MeshFrame_ mCurFrame;

	MeshFramePool_ mMeshFramePool;

	size_t mCurFrameIndex;

	enum{
		VBO_VERTEX,
		VBO_NORMAL,
		VBO_INDEX,
		VBO_SIZE
	};
	unsigned int mHumanVBO[VBO_SIZE];
	unsigned int mGarmentVBO[VBO_SIZE];
private:

	Visualizer(void);

	static void OnReshape(int nw, int nh);

	static void OnRender();

	static void OnMouseDown(int button, int s, int x, int y);

	static void OnMouseMove(int x, int y);

	void DrawGrid();

	void drawMesh();

	void setLight();

	static void tick(int val);

	void prepare( Mesh& _mesh, unsigned int* _vbo);

public:	

	~Visualizer(void);

	void show(int argc, char** argv);	

	static Visualizer_ getInstance();

	void setHumanMesh(Mesh_ mesh);
	
	void setGarmentMesh(Mesh_ mehs);	

	void initGL();
public:
	/* ---------------------------GarmentSimulationCallBack-----------------------------------*/
	/* 开始模拟 */
	void onSimulateBegin();

	/* 每计算出一帧时调用，这个函数不应该执行耗时的操作，不然会拖慢模拟过程 */
	void onFrame(MeshFrame_ frame);

	/* 结束模拟 */
	void onSimulateEnd(MeshFramePool_ mMeshFramePool);
	/* ---------------------------GarmentSimulationCallBack-----------------------------------*/
};

