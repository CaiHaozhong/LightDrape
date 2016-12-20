#pragma once
#include "Common.h"
#include <vector>
#include "Vec3d.h"
class VertexAlter {
private:
	std::vector<Vec3d> mVertexDelta;
public:
	VertexAlter(void);
	~VertexAlter(void);

	void resize(size_t size){
		mVertexDelta.resize(size);
	}

	/* 增加一个顶点的位置变化 */
	bool add(size_t index, const Vec3d& delta){
		if(mVertexDelta.size() == 0){			
			return false;
		}
		mVertexDelta[index] = Vec3d(delta);
	}

	Vec3d& getDelta(size_t index){
		return mVertexDelta[index];
	}
};
S_PTR(VertexAlter);