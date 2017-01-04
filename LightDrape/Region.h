#pragma once
#include <vector>
#include "Common.h"
#include "Skeleton.h"
#include <set>
#include <queue>
class Region
{
private:
	std::set<size_t> mVertics;
	std::set<size_t> mSkeNodes;
	Skeleton_ mSkeleton;

	/* ��Region����ʼ�����ڵ� */
	size_t mStartIndex;
public:
	Region(void);
	~Region(void);
	Region(Skeleton_ ske){
		setSkeleton(ske);
	}
	void setSkeleton(Skeleton_ ske){
		mSkeleton = ske;
	}

	void addVertex(size_t vertex){
		mVertics.insert(vertex);
	}

	void addSkeleton(size_t skenode){
		mSkeNodes.insert(skenode);
	}

	/* ���ø�Region����ʼ�����ڵ� */
	void setStart(size_t start){
		mStartIndex = start;
	}

	std::set<size_t>& getVertices() { return mVertics; };
	std::set<size_t>& getSkeNodes() { return mSkeNodes; }

	/* �ѹ����ڵ��Ľڵ�����Region
	 * ���ѹ����ڵ��Ӧ�����񶥵�����Region
	 * �Թ����ڵ㰴�����ӹ�ϵ��������
	 */
	void expand(){
		if(mSkeleton == nullptr)
			return;
		
		size_t nodeCount = mSkeleton->nodeCount();
		bool* inRegion = new bool[nodeCount];
		memset(inRegion, false, nodeCount);
		size_t curSize = mSkeNodes.size();
		for(std::set<size_t>::iterator it = mSkeNodes.begin();
			it != mSkeNodes.end(); it++){
			inRegion[*it] = true;
		}
		bool* hasVisited = new bool[nodeCount];
		memset(hasVisited, false, nodeCount);
		size_t* pre = new size_t[nodeCount];		
		std::queue<size_t> Q;
		size_t source = *(mSkeNodes.begin());
		hasVisited[source] = true;
		Q.push(source);
		while(!Q.empty()){
			size_t cur = Q.front();
			Q.pop();
			Skeleton::IndexList& neighbors = mSkeleton->getNeighbors(cur);
			size_t neiCount = neighbors.size();
			for(size_t i = 0; i < neiCount; i++){
				size_t nei = neighbors[i];
				if(hasVisited[nei]){
					continue;
				}
				if(!inRegion[nei]){
					pre[nei] = cur;
				}
				else{
					size_t preNode = cur;
					while(!inRegion[preNode]){
						mSkeNodes.insert(preNode);
						inRegion[preNode] = true;
						preNode = pre[preNode];
					}
				}
				hasVisited[nei] = true;
				Q.push(nei);
			}
		}
		sortSkeleton();
		expandVertices();
		delete [] pre;
		delete [] hasVisited;
		delete [] inRegion;
	}

private:
	void sortSkeleton(){
		/* TODO */
	}

	void expandVertices(){		
		size_t vCount = mVertics.size();
		size_t skeCount = mSkeNodes.size();			
		for(std::set<size_t>::iterator it = mSkeNodes.begin();
			it != mSkeNodes.end(); it++){
			std::vector<size_t>& corrs = mSkeleton->nodeAt(*it)->correspondanceIndices;
			size_t corCount = corrs.size();
			for(size_t j = 0; j < corCount; j++){
				mVertics.insert(corrs[j]);
			}
		}
	}
};
S_PTR(Region);