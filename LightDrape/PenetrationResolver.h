#pragma once
#include "Common.h"
#include <vector>
class Mesh;
S_PTR(Mesh);
/* 移除衣服网格与人体网格之间的穿透
 * 不改变人体网格
 * 会改变衣服网格顶点的位置
 */
class PenetrationResolver
{
protected:
	Mesh_ mGarment;
	Mesh_ mHuman;
	double mEPS;
public:
	PenetrationResolver(void);
	PenetrationResolver(Mesh_ garment);
	~PenetrationResolver(void);
	void setGarmentMesh(Mesh_ garment);
	void setHumanMesh(Mesh_ human);

	/* 进行穿透调整 */
	bool resolve();

	/* 设置服装与人体皮肤的距离， 单位为cm，默认为0.5cm */
	void setGarHumDis(double eps);
protected:
	/* 计算服装的每个网格顶点对应的距离最近的人体网格顶点 
	 * 结果保存在nnsHumanVertex中，元素为人体网格顶点的下标
	 */ 
	void computeNNSHumanVertex(std::vector<int>& nnsHumanVertex );

	/* 计算得到与人体有穿透的衣服网格顶点，保存在布尔数组penetrationTest中
	 * penetrationTest 的大小等于衣服顶点个数，且数组元素初始化为0 */
	void computePenetrationVertices(const std::vector<int>& nearestHumanVertex, std::vector<bool>& penetrationTest );

	/* 计算网格顶点的邻接表 */
	void computeAdjList( Mesh_ mesh, std::vector< std::vector<size_t> >& adjList );
};
S_PTR(PenetrationResolver);

