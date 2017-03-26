#pragma once
#include "regionfitter.h"
#include <set>
class WatertightMesh;
S_PTR(WatertightMesh);
class Skeleton;
S_PTR(Skeleton);
class VertexAlter;
S_PTR(VertexAlter);
class MultiNextNodeHandler{
protected:
	Skeleton_ mSkeleton;
public:
	MultiNextNodeHandler(Skeleton_ skeleton){
		mSkeleton = skeleton;
	}
	virtual size_t decide(size_t prev, std::vector<size_t>& nextNodes) = 0;
};
S_PTR(MultiNextNodeHandler);
class ChooseLongBranch : public MultiNextNodeHandler{
public:
	ChooseLongBranch(Skeleton_ skeleton):MultiNextNodeHandler(skeleton){}

	size_t decide(size_t prev, std::vector<size_t>& nextNodes);
private:
	size_t length(size_t prev, size_t node);
};
S_PTR(ChooseLongBranch);

class ChooseRadomBranch : public MultiNextNodeHandler{
public:
	ChooseRadomBranch(Skeleton_ skeleton):MultiNextNodeHandler(skeleton){}

	size_t decide(size_t prev, std::vector<size_t>& nextNodes);
private:
};
S_PTR(ChooseRadomBranch);
class SkeletonFitter :
	public RegionFitter
{
public:
	SkeletonFitter(void);
	SkeletonFitter(Region_ garmentRegion);
	~SkeletonFitter(void);

protected:
	VertexAlter_ fit(Region_ humanRegion);

	/* 根据人体的区域来得到衣服区域的骨骼节点的位移量 
	 * 参数 garSkeTrans 为输出
	 * garSkeTrans中的骨骼节点按照连接关系从开始节点按顺序存储
	 */
	void fitSkeleton(std::vector< std::pair<size_t, Vec3d> >& garSkeTrans, 
		std::vector< std::pair<size_t, std::pair<Vec3d, double> > >& garSkeRotate,
		Region_ humanRegion);

	/* 加权计算骨骼节点对应的网格顶点的位移
	 * 每个网格顶点受其对应的骨骼节点及相邻两个骨骼节点的影响，边界则为一个
	 * 按网格顶点与骨骼节点的距离加权平均
	 */
	void computeTranslation(VertexAlter_ ret, Region_ region, std::vector< std::pair<size_t, Vec3d> >& garSkeTrans);

	/* 对骨骼节点对应的网格顶点进行旋转 */
	void computeRotation(VertexAlter_ ret, Region_ region, 
		std::vector< std::pair<size_t, std::pair<Vec3d, double> > >& garSkeRotate);

	/* 对骨骼节点按照连接关系进行排序
	 * 参数：sortedSkeNodes 输出
	 * 参数：multiNextNodeHandler 当一个节点的下一个节点有多个可选时，由这个对象
	 * 决定选哪个
	 */
	virtual void getSortedSkeleton(Region_ region, std::vector<size_t>& sortedSkeNodes,
		MultiNextNodeHandler_ multiNextNodeHandler);

	/* 获取起始骨骼节点 */
	virtual size_t getStartSkeletonNode(Region_ region) = 0;

	/* 获取该Region的骨骼 */
	virtual Skeleton_ getSkeleton(Region_ region) = 0;

	/* 获取处于该region中的骨骼节点的集合 */
	virtual std::set<size_t>& getSkeletonNodesInRegion(Region_ region) = 0;

	void dumpDeformSkeleton(std::vector< std::pair<size_t, Vec3d> >& garSkeTrans);
};

