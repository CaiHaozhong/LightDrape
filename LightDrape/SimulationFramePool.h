#pragma once
#include "Common.h"
/* 存放物理模拟得到的帧的场所 */
class SimulationFramePool
{
public:
	SimulationFramePool(void);
	~SimulationFramePool(void);
};
S_PTR(SimulationFramePool);
