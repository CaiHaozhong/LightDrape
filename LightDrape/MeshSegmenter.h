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
	/* Level Set֮��ļ�� */
	double mGranularity;
	
	/* �ж����񶥵��Ƿ��Ѿ�����Region��
	 * TODO������һ���Ƿ������ */ 
	BooleanProperty_ hasAdded;
	std::vector<bool> hasSkeletonNodeAdded;//�жϹ����ڵ��Ƿ��Ѿ�����Region��
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


	/* ��ʼ�ָ�����ָ����洢��Mesh�� */
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
	/* ��������ߵĳ��Ⱦ���Level Set�ļ�� 
	 * ���Ϊ�������бߵ�ƽ�����ȵ�һ��
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
				/* ���������Ҷ���ȷ��������������ϴ�����򵥲�ֵ */
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

	/* ��ͬ��ģ�ͣ��ָ�ķ�����ͬ
	 * ������isNoise������˷���������ȷ��LevelSet
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

	/* ��ȡһ���������˵��У���غ���ֵ��С����һ�� */
	double getMinDisFromEdge(Mesh::EdgeHandle edge){
		std::pair<size_t, size_t> vs = mMesh->getEndVertices(edge);
		double minDis = std::min(mGeodisPropery->get(vs.first),
			mGeodisPropery->get(vs.second));
		return minDis;
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

	/* ��һ��LevelCircle�ӵ�һ��Region�� */
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
	/* ��ȡһ��LevelCircle��Ӧ�Ĺ����ڵ�
	 * ���أ���Ӧ���LevelCircle�������ڵ�Ĺ����ڵ� 
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

