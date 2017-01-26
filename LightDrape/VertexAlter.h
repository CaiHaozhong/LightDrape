#pragma once
#include "Common.h"
#include <vector>
#include <tuple>
#include "Vec3d.h"
class VertexAlter {
public:
	std::vector<std::pair<size_t, Vec3d> > vertexDelta;
public:
	VertexAlter(void);
	~VertexAlter(void);

	/* 增加一个顶点的形变变化量
	 * 返回增加后vertexDelta的大小
	 */
	size_t add(size_t index, const Vec3d& delta);

	size_t size() const;

	size_t getIndex(size_t i) const;

	Vec3d& getDelta(size_t i);

	std::pair<size_t, Vec3d>& getAlter(size_t i);
};
S_PTR(VertexAlter);