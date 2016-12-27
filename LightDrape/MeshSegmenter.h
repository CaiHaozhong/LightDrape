#pragma once
#include "WatertightMesh.h"
#include "LevelSet.h"
#include "GeodesicResolver.h"
#include "Segment.h"
#include <vector>
class MeshSegmenter
{
private:
	WatertightMesh_ mMesh;
	std::vector<LevelSet> mLevelSets;
	DoubleProperty_ mGeodisPropery;
	Segment_ mSegment;
	/* Level Set֮��ļ�� */
	double mGranularity;
public:
	MeshSegmenter(void);
	MeshSegmenter(WatertightMesh_ mesh){
		init(mesh);		
	}
	~MeshSegmenter(void);
	void init(WatertightMesh_ mesh){
		mMesh = mesh;
		decideGranularity();
		GeodesicResolver geodesicResolver;
		mGeodisPropery = geodesicResolver.resolveGeo(mMesh);
		computeLevelSet();
	}		


	/* ��ʼ�ָ�����ָ����洢��Mesh�� */
	void segment(){
		mSegment = createSegment();
		std::vector<bool> isNoise;		
		filterNoise(isNoise);
		coarseSegment(isNoise);
		refineSegment();
		mMesh->setSegment(mSegment);
	}
	WatertightMesh_ getMesh() const { return mMesh; }
private:
	/* ��������ߵĳ��Ⱦ���Level Set�ļ�� */
	void decideGranularity(){
		mGranularity = 20;
	}

	void computeLevelSet(){

	}

	void refineSegment();

	/* ��ͬ��ģ�ͣ��ָ�ķ�����ͬ
	 * ������isNoise������˷���������ȷ��LevelSet
	 */
	void coarseSegment(std::vector<bool>& isNoise){
		size_t len = mLevelSets.size();
		size_t seq = 1;
		size_t i = 0;		
		while(isNoise[i]){
			++i;
		}
		size_t last = mLevelSets[i].getCount();
		for( ; i < len; ){
			size_t cur = mLevelSets[i].getCount();
			if(cur != last){
				++seq;
				last = cur;
			}
			onDifferentLevelSet(seq, mLevelSets[i]);
			++i;
			while(isNoise[i]){
				++i;
			}
		}
	}
protected:

	/* ��ͬ��������LevelSet�Ļص�����
	   MeshSegment�ӵ�һ��LevelSet��ʼ���������ϵػص��������
	   seq��ʾ��ͬ��LevelSet���ֵ�˳�򣬴�1��ʼ
	   �������ʵ���������
	 */
	virtual void onDifferentLevelSet(size_t seq, LevelSet& levelSet) = 0;

	/* ��ͬ��ģ�ͣ��в�ͬ�ķָ�
	   �������ʵ���������
	 */
	virtual Segment_ createSegment() = 0;

	/* ȥ��Level Set����������
	 * ��������ṹ�ǲ�����ģ���ЩLevel Set������������ǲ��Եģ���Ҫ����ȥ��
	 * ����Ӽ���ֱۣ����ܳ��ָ���Ϊ2��Level Set����ô����Ҫ��2ȥ��
	 * isNoiseΪ����ֵ����С��mLevelSet�Ĵ�Сһ�������������������LevelSet��ǳ���
	 */
	virtual void filterNoise(std::vector<bool>& isNoise){
		isNoise.resize(mLevelSets.size(), false);
		size_t len = mLevelSets.size();
		int threshold = 3; // ��������С�ڵ���threshold�Ķ�������
		int val = mLevelSets[0].getCount();
		int accu = 1;
		int start = 0;
		for(size_t i = 1; i < len; i++){
			int cur = mLevelSets[i].getCount();
			if(cur == val){
				++accu;
			}
			else{				
				if(accu <= threshold){
					while(accu--){
						isNoise[start++] = true;
					}
				}
				start = i;
				val = cur;
				accu = 1;
			}			
		}
		if(accu <= threshold){
			while(accu--){
				isNoise[start++] = true;
			}
		}
	}
};

