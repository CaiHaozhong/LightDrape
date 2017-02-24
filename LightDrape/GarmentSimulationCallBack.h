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

	/* ��ʼģ�� */
	virtual void onSimulateBegin() = 0;

	/* ÿ�����һ֡ʱ���ã����������Ӧ��ִ�к�ʱ�Ĳ�������Ȼ������ģ����� */
	virtual void onFrame(MeshFrame_ frame) = 0;

	/* ����ģ�� */
	virtual void onSimulateEnd(MeshFramePool_ mMeshFramePool) = 0;

};
S_PTR(GarmentSimulationCallBack);

