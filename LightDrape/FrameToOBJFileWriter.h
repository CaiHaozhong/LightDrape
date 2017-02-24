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

	/* ��ʼģ�� */
	void onSimulateBegin();

	/* ÿ�����һ֡ʱ���ã����������Ӧ��ִ�к�ʱ�Ĳ�������Ȼ������ģ����� */
	void onFrame(MeshFrame_ frame);

	/* ����ģ�� */
	void onSimulateEnd(MeshFramePool_ mMeshFramePool);


private:
	void writeFrameToFile(MeshFrame_ frame, size_t i);
};
S_PTR(FrameToOBJFileWriter);
