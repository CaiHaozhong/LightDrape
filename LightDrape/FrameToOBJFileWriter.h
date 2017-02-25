#pragma once
#include "garmentsimulationcallback.h"
#include <fstream>
class Mesh;
S_PTR(Mesh);
class FrameToOBJFileWriter :
	public GarmentSimulationCallBack
{
private:
	std::ofstream out;
	size_t mFrameCount;
public:
	FrameToOBJFileWriter();
	~FrameToOBJFileWriter(void);

	/* ��ʼģ�� */
	void onSimulateBegin();

	/* ÿ�����һ֡ʱ���ã����������Ӧ��ִ�к�ʱ�Ĳ�������Ȼ������ģ����� */
	void onFrame(MeshFrame_ frame);

	/* ����ģ�� */
	void onSimulateEnd(MeshFramePool_ mMeshFramePool);


private:
	void writeFrameToFile(MeshFrame_ frame);
};
S_PTR(FrameToOBJFileWriter);
