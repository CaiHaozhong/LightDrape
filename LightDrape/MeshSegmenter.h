#pragma once
#include "WatertightMesh.h"
#include "LevelSet.h"
#include "GeodesicResolver.h"
#include "GeodesicResolverCached.h"
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
	
	/* 判断网格顶点是否已经加入Region了
	 * TODO：测试一下是否回收了 */ 
	BooleanProperty_ hasAdded;
	std::vector<bool> hasSkeletonNodeAdded;//判断骨骼节点是否已经加入Region了
public:
	MeshSegmenter(void);
	MeshSegmenter(WatertightMesh_ mesh){
		init(mesh);		
	}
	~MeshSegmenter(void);
	virtual void init(WatertightMesh_ mesh){
		mMesh = mesh;
		decideGranularity();
		PRINTLN("Begin compute Geodesic...");
		GeodesicResolver_ geodesicResolver = smartNew(GeodesicResolverCached);
		mGeodisPropery = geodesicResolver->resolveGeo(mMesh);
		PRINTLN("End compute Geodesic...");
		hasAdded = smartNew(BooleanProperty);
		mMesh->registerProperty(hasAdded);
		for(size_t i = 0; i < mMesh->n_vertices(); i++){
			hasAdded->set(i, false);
		}
		hasSkeletonNodeAdded.resize(mMesh->getSkeleton()->nodeCount(), false);
		PRINTLN("Begin computeLevelSet...");
		computeLevelSet();
		PRINTLN("End computeLevelSet...");
	}		


	/* 开始分割，并将分割结果存储在Mesh中 */
	void segment(){
		mSegment = createSegment();
		std::vector<bool> isNoise;		
		PRINTLN("Begin filterNoise...");
		filterNoise(isNoise);
		PRINTLN("End filterNoise...");
		coarseSegment(isNoise);
		refineSegment();
		mMesh->setSegment(mSegment);
	}
	WatertightMesh_ getMesh() const { return mMesh; }
private:
	/* 根据网格边的长度决定Level Set的间隔 
	 * 间隔为网格所有边的平均长度的一半
	 */
	void decideGranularity(){
		double edgeLengthSum = 0;
		for(Mesh::EdgeIter ei = mMesh->edges_begin();
			ei != mMesh->edges_end(); ei++){
				edgeLengthSum += mMesh->getEdgeLength(*ei);
		}
		mGranularity = edgeLengthSum/mMesh->n_edges()*2;
	}

	void computeLevelSet(){
		std::vector<Mesh::EdgeHandle> meshEdges;
		for(Mesh::EdgeIter eit = mMesh->edges_begin(); 
			eit != mMesh->edges_end();eit++){
				meshEdges.push_back(*eit);
		}
		std::sort(meshEdges.begin(), meshEdges.end(),[this](Mesh::EdgeHandle& a,
			Mesh::EdgeHandle& b){				
				double minDisA = getMinDisFromEdge(a);
				double minDisB = getMinDisFromEdge(b);
				return minDisA < minDisB;
		});
		double curLevel = mGranularity;
		size_t cursor = 0;
		mLevelSets.clear();
		mLevelSets.reserve(32);
		mLevelSets.push_back(LevelSet());
		LevelSet* levelSet = &mLevelSets[mLevelSets.size()-1];
		while(cursor < meshEdges.size()){						
			Mesh::EdgeHandle e = meshEdges[cursor];
			std::pair<size_t, size_t> vs = mMesh->getEndVertices(e);
			std::pair<size_t, double> verDisPair01 
				= std::make_pair(vs.first, mGeodisPropery->get(vs.first));
			std::pair<size_t, double> verDisPair02
				= std::make_pair(vs.second, mGeodisPropery->get(vs.second));
			if(verDisPair01.second > verDisPair02.second){
				std::swap(verDisPair01, verDisPair02);
			}
			if(verDisPair01.second < curLevel && verDisPair02.second > curLevel){
				LevelNode_ node = smartNew(LevelNode);
				node->edge = e.idx();
				node->start_vertex = verDisPair01.first;
				/* 可以用余弦定理精确求出，但计算量较大，这里简单插值 */
				node->factor = (curLevel-verDisPair01.second)
					/(verDisPair02.second-verDisPair01.second);	
				levelSet->addNode(node);
				++cursor;
			}
			else if(verDisPair01.second > curLevel){
				curLevel += mGranularity;
				mLevelSets.push_back(LevelSet());
				levelSet = &mLevelSets[mLevelSets.size()-1];
			}
			else{
				++cursor;
			}			
		}
		//mLevelSets[19].dumpRaw(mMesh, 19);
// 		mLevelSets[19].init(mMesh);
// 		mLevelSets[19].dump(19);
		for(size_t i = 0; i < mLevelSets.size(); i++){
			mLevelSets[i].init(mMesh);			
			mLevelSets[i].dump(i);
		}
		
		char c[20];
		sprintf(c, "%d", mLevelSets.size());
		PRINTLN(std::string("LevelSet Count ") + c);
	}

	void refineSegment(){
		/* TO DO
		 */ 
	}

	/* 不同的模型，分割的方法不同
	 * 参数：isNoise，标记了分类数不正确的LevelSet
	 */
	void coarseSegment(std::vector<bool>& isNoise){
		size_t len = mLevelSets.size();
		size_t seq = 1;
		size_t i = 0;		
		while(i < len && isNoise[i]){
			++i;
		}
		size_t last = 0;
		if(i < len)
		last = mLevelSets[i].getCount();
		for( ; i < len; ){
			size_t cur = mLevelSets[i].getCount();
			if(cur != last){
				++seq;
				last = cur;
			}
			onDifferentLevelSet(seq, mLevelSets[i]);
			++i;
			while(i < len && isNoise[i]){
				++i;
			}
		}
	}

	/* 获取一条边两个端点中，测地函数值最小的那一个 */
	double getMinDisFromEdge(Mesh::EdgeHandle edge){
		std::pair<size_t, size_t> vs = mMesh->getEndVertices(edge);
		double minDis = std::min(mGeodisPropery->get(vs.first),
			mGeodisPropery->get(vs.second));
		return minDis;
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

	/* 将一个LevelCircle加到一个Region中 */
	void addToRegion(Region_ region, LevelCircle_ levelCircle){
		std::vector<LevelNode_> levelNodes = levelCircle->levelNodes;		
		for(size_t i = 0; i < levelNodes.size(); i++){
			LevelNode_ n = levelNodes[i];
			size_t v = n->getNearestVertex(mMesh);
			if(!hasAdded->get(v)){
				region->addVertex(v);
				hasAdded->set(v, true);
			}
			size_t skenode = mMesh->getCorrSkeletonNode(v);
			if(hasSkeletonNodeAdded[skenode] == false){
				region->addSkeleton(skenode);
				hasSkeletonNodeAdded[skenode] = true;
			}
		}
	}
	/* 获取一个LevelCircle对应的骨骼节点
	 * 返回：对应这个LevelCircle中最多个节点的骨骼节点 
	 */
	size_t getCircleSkeletonNode(LevelCircle_ levelCircle){		
		std::map<size_t, size_t> nodeCountMap;
		for(size_t i = 0; i < levelCircle->levelNodes.size(); i++){
			LevelNode_ n = levelCircle->levelNodes[i];
			size_t v = n->getNearestVertex(mMesh);
			size_t skeNode = mMesh->getCorrSkeletonNode(v);
			if(nodeCountMap.find(skeNode) == nodeCountMap.end()){
				nodeCountMap[skeNode] = 1;
			}
			else{
				nodeCountMap[skeNode] += 1;
			}
		}
		int maxCount = -1;
		size_t ret = -1;
		for(std::map<size_t, size_t>::iterator it = nodeCountMap.begin();
			it != nodeCountMap.end(); it++){
				if(it->second > maxCount){
					maxCount = it->second;
					ret = it->first;
				}
		}
		if(ret == -1){
			PRINTLN("ERROR! In getCircleSkeletonNode. ret == -1");
		}
		return -1;
	}
};

