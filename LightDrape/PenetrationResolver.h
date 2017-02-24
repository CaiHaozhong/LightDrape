#pragma once
#include "Common.h"
#include "Vec3d.h"
#include <vector>
class Mesh;
S_PTR(Mesh);
class KNNSHelper;
S_PTR(KNNSHelper);
/* 移除顶点列表与刚体网格之间的穿透
 * 不改变刚体网格
 */
class PenetrationResolver
{
private:
	Mesh_ mRigidMesh;

	/* 顶点与刚体表面的距离 */
	double mEPS;

	/* 目标顶点的连接表 */

	std::vector<std::vector<size_t> > mTargetAdjList;

	/* 用来完成最近邻搜索 */
	KNNSHelper_ mKNNSHelper;
public:
	PenetrationResolver(void);
	~PenetrationResolver(void);

	/* 设置刚体网格 */
	void setRigidMesh(Mesh_ rigid);

	/* 设置目标顶点的连接关系 */
	void setAdjList(std::vector<std::vector<size_t> >& adjList);

	/* 进行穿透调整
	 * 参数：顶点数据
	 * 其连接关系由setAdjList(std::vector<std::vector<size_t> >&)给出
	 * penetration：返回解决穿透之前，有穿透的顶点
	 * 返回值：如果有穿透，则返回false，否则返回true
	 */
	bool resolve(std::vector<Vec3d>& points, std::vector<size_t>& penetrationVers = std::vector<size_t>());

	/* 设置服装与人体皮肤的距离， 单位为cm，默认为0.5cm */
	void setAllowDis(double eps);
private:
	/* 计算服装的每个网格顶点对应的距离最近的人体网格顶点 
	 * 结果保存在nnsHumanVertex中，元素为人体网格顶点的下标
	 */ 
	void computeNNSHumanVertex(std::vector<int>& nnsHumanVertex, std::vector<Vec3d>& points );

	/* 计算得到与人体有穿透的衣服网格顶点，保存在布尔数组penetrationTest中
	 * penetrationTest 的大小等于衣服顶点个数，且数组元素初始化为0
	 * 返回值：如果没有穿透的顶点，则返回false，否则返回true
	 */
	bool computePenetrationVertices(const std::vector<int>& nearestHumanVertex, std::vector<bool>& penetrationTest, std::vector<Vec3d>& points );

};
S_PTR(PenetrationResolver);

