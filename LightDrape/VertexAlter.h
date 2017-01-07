#pragma once
#include "Common.h"
#include <vector>
#include <tuple>
#include "Vec3d.h"
class VertexAlter {
public:
	std::vector<std::pair<size_t, Vec3d> > vertexDelta;
	Vec3d trans;
public:
	VertexAlter(void);
	~VertexAlter(void);

	/* ����һ��������α�仯��
	 * �������Ӻ�vertexDelta�Ĵ�С
	 */
	size_t add(size_t index, const Vec3d& delta);

	void translate(const Vec3d& delta);

};
S_PTR(VertexAlter);