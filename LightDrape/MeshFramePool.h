#pragma once
#include <vector>
#include "Vec3d.h"
#include "Common.h"
class Mesh;
S_PTR(Mesh);
/* ֡���������񶥵��λ�� */
class MeshFrame{
private:
	std::vector<Vec3d> vers;
public:
	MeshFrame();

	MeshFrame(std::vector<Vec3d>& points);

	MeshFrame(Mesh_ mesh);

	void initFromMesh(Mesh_ mesh);

	void initFromPoints(std::vector<Vec3d>& points);

	std::vector<Vec3d>& getVertices();
};
S_PTR(MeshFrame);
/* �������ģ��õ���֡�ĳ��� */
class MeshFramePool
{
private:
	std::vector<MeshFrame_> mFrames;

	/* ֡�ʣ�֡/�� */
	size_t mFrameRate;

public:
	MeshFramePool(void);
	~MeshFramePool(void);

	/* ����һ֡ */
	void storeFrame(MeshFrame_ frame);
	
	/* ����һ֡ */
	void storeFrame(std::vector<Vec3d>& frame);

	/* ����һ֡ */
	void storeFrame(Mesh_ frame);

	/* ����֡�ʣ�Ĭ����25 */
	void setFPS(size_t fps);

	size_t getFPS() const;

	MeshFrame_ getFrame(size_t index);

	size_t getFrameCount() const;

};
S_PTR(MeshFramePool);
