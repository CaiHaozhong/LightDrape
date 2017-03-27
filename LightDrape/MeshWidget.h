#pragma once
#include "GarmentSimulationCallBack.h"
#include "MeshSegmentListener.h"
#include "Common.h"
#include "qglviewerwidget.h"
class Mesh;
class Human;
class Garment;
class MeshFramePool;
class VisibleMesh;
S_PTR(Mesh);
S_PTR(Human);
S_PTR(Garment);
S_PTR(MeshFramePool);
S_PTR(VisibleMesh);
class MeshWidget :
	public QGLViewerWidget, 
	public GarmentSimulationCallBack, 
	public MeshSegmentListener,
	public std::enable_shared_from_this<MeshWidget>
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

public:
	void onEndCoarseSegment(Segment_ seg);

private:
	std::weak_ptr<Human> mHuman;
	Garment_ mGarment;
	VisibleMesh_ mVisibleHuman, mVisibleGarment;
	std::vector<VisibleMesh_> mVisbleMeshes;
	MeshFramePool_ mMeshFramePool;
	int mTimerID;
	int mCurFrameIndex;
	int mFrameCount;
	
};
S_PTR(MeshWidget);