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
	/* Level Set֮��ļ�� */
	double mGranularity;
	
protected:
	Segment_ mSegment;
	std::vector<MeshSegmentListener_> mListeners;
public:
	MeshSegmenter(void);
	MeshSegmenter(WatertightMesh_ mesh);
	~MeshSegmenter(void);
	virtual void init(WatertightMesh_ mesh);		


	/* ��ʼ�ָ�����ָ����洢��Mesh�� */
	void segment();
	WatertightMesh_ getMesh() const;

	size_t getLevelSetCount() const;

	LevelSet_ getLevelSet(size_t i);

	/* �����LevelSet��mLevelSets�е��±� */
	size_t getLevelSetIndex(LevelSet_ ls);

	double getGranularity() const;

	void addSegmentListener(MeshSegmentListener_ listener);

	void addSegmentListener(const std::vector<MeshSegmentListener_>& listeners);
private:
	/* ��������ߵĳ��Ⱦ���Level Set�ļ�� 
	 * ���Ϊ�������бߵ�ƽ�����ȵ�һ��
	 */
	void decideGranularity();

	void computeLevelSet(bool useCache = false);
	

	/* ��ͬ��ģ�ͣ��ָ�ķ�����ͬ
	 * ������isNoise������˷���������ȷ��LevelSet
	 */
	void coarseSegment(std::vector<bool>& isNoise);

	/* ��ȡһ���������˵��У���غ���ֵ��С����һ�� */
	double getMinDisFromEdge(Mesh::EdgeHandle edge);
protected:

	/* ��ͬ��������LevelSet�Ļص�����
	   MeshSegment�ӵ�һ��LevelSet��ʼ���������ϵػص��������
	   seq��ʾ��ͬ��LevelSet���ֵ�˳�򣬴�1��ʼ
	   �������ʵ���������
	 */
	virtual void onDifferentLevelSet(size_t seq, LevelSet_ levelSet) = 0;


	/* ���Էָ�����Ļص��������������е�LevelSet����� */
	virtual void onFinishCoarseSegment();

	/* ��ͬ��ģ�ͣ��в�ͬ�ķָ�
	   �������ʵ���������
	 */
	virtual Segment_ createSegment() = 0;

	/* ȥ��Level Set����������
	 * ��������ṹ�ǲ�����ģ���ЩLevel Set������������ǲ��Եģ���Ҫ����ȥ��
	 * ����Ӽ���ֱۣ����ܳ��ָ���Ϊ2��Level Set����ô����Ҫ��2ȥ��
	 * isNoiseΪ����ֵ����С��mLevelSet�Ĵ�Сһ�������������������LevelSet��ǳ���
	 */
	virtual void filterNoise(std::vector<bool>& isNoise);

	/* ��һ��LevelCircle�ӵ�һ��Region�� */
	void addToRegion(Region_ region, LevelCircle_ levelCircle);
	/* ��ȡһ��LevelCircle��Ӧ�Ĺ����ڵ�
	 * ���أ���Ӧ���LevelCircle�������ڵ�Ĺ����ڵ� 
	 */
	size_t getCircleSkeletonNode(LevelCircle_ levelCircle);

	/* �����ָ�Ļص� */
	virtual void onFinishSegmentHook();

	/* ��ʼ�ָ�Ļص� */
	virtual void onBeginSegmentHook();

	/* ��һ���ָ� */
	virtual void refineSegment() = 0;


	void handleNoise(LevelSet_ ls, std::vector<Region_>& curRegions);

};
S_PTR(MeshSegmenter);