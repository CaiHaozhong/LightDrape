#pragma once
#include <vector>
#include "Vec3d.h"
#include "Common.h"
class Mesh;
S_PTR(Mesh);
/* 帧，保存网格顶点的位置 */
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
/* 存放物理模拟得到的帧的场所 */
class MeshFramePool
{
private:
	std::vector<MeshFrame_> mFrames;

	/* 帧率：帧/秒 */
	size_t mFrameRate;

public:
	MeshFramePool(void);
	~MeshFramePool(void);

	/* 保存一帧 */
	void storeFrame(MeshFrame_ frame);
	
	/* 保存一帧 */
	void storeFrame(std::vector<Vec3d>& frame);

	/* 保存一帧 */
	void storeFrame(Mesh_ frame);

	/* 设置帧率，默认是25 */
	void setFPS(size_t fps);

	size_t getFPS() const;

	MeshFrame_ getFrame(size_t index);

	size_t getFrameCount() const;

};
S_PTR(MeshFramePool);
