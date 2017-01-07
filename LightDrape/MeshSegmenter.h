#pragma once
#include "WatertightMesh.h"
#include "LevelSet.h"
#include "GeodesicResolver.h"
#include "GeodesicResolverCached.h"
#include "Segment.h"
#include <vector>
#include "LevelSetCacher.h"
class MeshSegmenter
{
private:
	WatertightMesh_ mMesh;
	std::vector<LevelSet_> mLevelSets;
	DoubleProperty_ mGeodisPropery;
	Segment_ mSegment;
	/* Level Set֮��ļ�� */
	double mGranularity;
	
	/* �ж����񶥵��Ƿ��Ѿ�����Region��
	 * TODO������һ���Ƿ������ */ 
// 	BooleanProperty_ hasAdded;
// 	std::vector<bool> hasSkeletonNodeAdded;//�жϹ����ڵ��Ƿ��Ѿ�����Region��
public:
	MeshSegmenter(void);
	MeshSegmenter(WatertightMesh_ mesh);
	~MeshSegmenter(void);
	virtual void init(WatertightMesh_ mesh);		


	/* ��ʼ�ָ�����ָ����洢��Mesh�� */
	void segment();
	WatertightMesh_ getMesh() const;
private:
	/* ��������ߵĳ��Ⱦ���Level Set�ļ�� 
	 * ���Ϊ�������бߵ�ƽ�����ȵ�һ��
	 */
	void decideGranularity();

	void computeLevelSet(bool useCache = false);

	void refineSegment();

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


	/* �ָ�����Ļص��������������е�LevelSet */
	virtual void onFinishCoarseSegment() = 0;

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
};
S_PTR(MeshSegmenter);