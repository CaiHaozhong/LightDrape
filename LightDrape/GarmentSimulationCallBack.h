#pragma once
#include "Common.h"
class MeshFramePool;
S_PTR(MeshFramePool);
class MeshFrame;
S_PTR(MeshFrame);
class GarmentSimulationCallBack
{
public:
	GarmentSimulationCallBack(void);
	~GarmentSimulationCallBack(void);

	/* 开始模拟 */
	virtual void onSimulateBegin() = 0;

	/* 每计算出一帧时调用，这个函数不应该执行耗时的操作，不然会拖慢模拟过程 */
	virtual void onFrame(MeshFrame_ frame) = 0;

	/* 结束模拟 */
	virtual void onSimulateEnd(MeshFramePool_ mMeshFramePool) = 0;

};
S_PTR(GarmentSimulationCallBack);

