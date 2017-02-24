#pragma once
#include "garmentsimulationcallback.h"
class Mesh;
S_PTR(Mesh);
class FrameToOBJFileWriter :
	public GarmentSimulationCallBack
{
private:
	Mesh_ mMesh;
	size_t mCurFrame;
public:
	FrameToOBJFileWriter(Mesh_ mesh);
	~FrameToOBJFileWriter(void);

	/* 开始模拟 */
	void onSimulateBegin();

	/* 每计算出一帧时调用，这个函数不应该执行耗时的操作，不然会拖慢模拟过程 */
	void onFrame(MeshFrame_ frame);

	/* 结束模拟 */
	void onSimulateEnd(MeshFramePool_ mMeshFramePool);


private:
	void writeFrameToFile(MeshFrame_ frame, size_t i);
};
S_PTR(FrameToOBJFileWriter);
