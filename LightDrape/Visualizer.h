#pragma once
#include "Common.h"
#include "GarmentSimulationCallBack.h"
/*#include "winnt.h"*/
#include <thread>
class MeshFrame;
S_PTR(MeshFrame);
class MeshFramePool;
S_PTR(MeshFramePool);
class Visualizer;
static Visualizer* currentInstance;
class Visualizer : public GarmentSimulationCallBack
{
private:
	double mStartTime;
	double mCurrentTime;
	double mFPS;

	double mWidth, mHeight;
// 	LARGE_INTEGER frequency;        // ticks per second
// 	LARGE_INTEGER t1, t2;           // ticks
public:
	Visualizer(void);
	~Visualizer(void);

	void show(int argc, char** argv);

	void fun(){
		for(int i = 0; i < 1000; ++i)
		{
			std::cout << "hello world" << std::endl;
			//线程休眠，chrono是c++11的时间相关库。
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
	void display();
	void setLight();


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
S_PTR(Visualizer);

