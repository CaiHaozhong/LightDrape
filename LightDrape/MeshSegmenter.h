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
	/* Level Set之间的间隔 */
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


	/* 开始分割，并将分割结果存储在Mesh中 */
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
	/* 根据网格边的长度决定Level Set的间隔 */
	void decideGranularity(){
		mGranularity = 20;
	}

	void computeLevelSet(){

	}

	void refineSegment();

	/* 不同的模型，分割的方法不同
	 * 参数：isNoise，标记了分类数不正确的LevelSet
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

	/* 不同分类数的LevelSet的回调函数
	   MeshSegment从第一个LevelSet开始遍历，不断地回调这个函数
	   seq表示不同类LevelSet出现的顺序，从1开始
	   子类必须实现这个函数
	 */
	virtual void onDifferentLevelSet(size_t seq, LevelSet& levelSet) = 0;

	/* 不同的模型，有不同的分割
	   子类必须实现这个函数
	 */
	virtual Segment_ createSegment() = 0;

	/* 去除Level Set个数的噪音
	 * 由于网格结构是不规则的，有些Level Set的类别数可能是不对的，需要将它去除
	 * 比如从肩膀到手臂，可能出现个数为2的Level Set，那么就需要将2去除
	 * isNoise为返回值，大小与mLevelSet的大小一样，将类别数是噪音的LevelSet标记出来
	 */
	virtual void filterNoise(std::vector<bool>& isNoise){
		isNoise.resize(mLevelSets.size(), false);
		size_t len = mLevelSets.size();
		int threshold = 3; // 连续个数小于等于threshold的都算噪音
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

