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
			//�߳����ߣ�chrono��c++11��ʱ����ؿ⡣
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
	void display();
	void setLight();


	void initGL();
public:
	/* ---------------------------GarmentSimulationCallBack-----------------------------------*/
	/* ��ʼģ�� */
	void onSimulateBegin();

	/* ÿ�����һ֡ʱ���ã����������Ӧ��ִ�к�ʱ�Ĳ�������Ȼ������ģ����� */
	void onFrame(MeshFrame_ frame);

	/* ����ģ�� */
	void onSimulateEnd(MeshFramePool_ mMeshFramePool);
	/* ---------------------------GarmentSimulationCallBack-----------------------------------*/
};
S_PTR(Visualizer);

