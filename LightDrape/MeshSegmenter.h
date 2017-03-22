#pragma once
#include "WatertightMesh.h"
#include "LevelSet.h"
#include "Segment.h"
#include <vector>
#include <unordered_set>
class GeodesicResolver;
S_PTR(GeodesicResolver);
class MeshSegmentListener;
S_PTR(MeshSegmentListener);
class MeshSegmenter : public std::enable_shared_from_this<MeshSegmenter>
{
private:
	WatertightMesh_ mMesh;
	std::vector<LevelSet_> mLevelSets;
	GeodesicResolver_ mGeodesicResolver;	
	/* Level Set之间的间隔 */
	double mGranularity;
	
protected:
	Segment_ mSegment;
	std::vector<MeshSegmentListener_> mListeners;
public:
	MeshSegmenter(void);
	MeshSegmenter(WatertightMesh_ mesh);
	~MeshSegmenter(void);
	virtual void init(WatertightMesh_ mesh);		


	/* 开始分割，并将分割结果存储在Mesh中 */
	void segment();
	WatertightMesh_ getMesh() const;

	size_t getLevelSetCount() const;

	LevelSet_ getLevelSet(size_t i);

	/* 计算该LevelSet在mLevelSets中的下标 */
	size_t getLevelSetIndex(LevelSet_ ls);

	double getGranularity() const;

	void addSegmentListener(MeshSegmentListener_ listener);

	void addSegmentListener(const std::vector<MeshSegmentListener_>& listeners);
private:
	/* 根据网格边的长度决定Level Set的间隔 
	 * 间隔为网格所有边的平均长度的一半
	 */
	void decideGranularity();

	void computeLevelSet(bool useCache = false);
	

	/* 不同的模型，分割的方法不同
	 * 参数：isNoise，标记了分类数不正确的LevelSet
	 */
	void coarseSegment(std::vector<bool>& isNoise);

	/* 获取一条边两个端点中，测地函数值最小的那一个 */
	double getMinDisFromEdge(Mesh::EdgeHandle edge);
protected:

	/* 不同分类数的LevelSet的回调函数
	   MeshSegment从第一个LevelSet开始遍历，不断地回调这个函数
	   seq表示不同类LevelSet出现的顺序，从1开始
	   子类必须实现这个函数
	 */
	virtual void onDifferentLevelSet(size_t seq, LevelSet_ levelSet) = 0;


	/* 粗略分割结束的回调，即遍历完所有的LevelSet后调用 */
	virtual void onFinishCoarseSegment();

	/* 不同的模型，有不同的分割
	   子类必须实现这个函数
	 */
	virtual Segment_ createSegment() = 0;

	/* 去除Level Set个数的噪音
	 * 由于网格结构是不规则的，有些Level Set的类别数可能是不对的，需要将它去除
	 * 比如从肩膀到手臂，可能出现个数为2的Level Set，那么就需要将2去除
	 * isNoise为返回值，大小与mLevelSet的大小一样，将类别数是噪音的LevelSet标记出来
	 */
	virtual void filterNoise(std::vector<bool>& isNoise);

	/* 将一个LevelCircle加到一个Region中 */
	void addToRegion(Region_ region, LevelCircle_ levelCircle);
	/* 获取一个LevelCircle对应的骨骼节点
	 * 返回：对应这个LevelCircle中最多个节点的骨骼节点 
	 */
	size_t getCircleSkeletonNode(LevelCircle_ levelCircle);

	/* 结束分割的回调 */
	virtual void onFinishSegmentHook();

	/* 开始分割的回调 */
	virtual void onBeginSegmentHook();

	/* 进一步分割 */
	virtual void refineSegment() = 0;


	void handleNoise(LevelSet_ ls, std::vector<Region_>& curRegions);

};
S_PTR(MeshSegmenter);