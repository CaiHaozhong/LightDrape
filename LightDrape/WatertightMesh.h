#pragma once
#include "Mesh.h"
#include "Skeleton.h"
#include "Segment.h"
#include "VertexAlter.h"
#include "HoleInfo.h"
#include <fstream>
class WatertightMesh :
	public Mesh
{
private:
	Skeleton_ mSkeleton;
	Segment_ mSegment;
	Mesh_ mOriginalMesh;
	VertexAlter_ mVertexAlter;
	
	/* 每一个网格顶点有一个存储该顶点对应的骨骼节点下标的属性 */
	IntProperty_ mVertexPropertySKN;

	/* 每个网格顶点有一个存储该顶点与测地源点测地距离的属性 */
	DoubleProperty_ mVertexPropertyGeoDis;

public:
	WatertightMesh(Mesh_ mesh):Mesh(*mesh){		
		mOriginalMesh = mesh;
		HoleInfo holeInfo(this);
		holeInfo.getHoles();
		PRINT("holes Count: ");
		PRINTLN(holeInfo.holes()->size());
		holeInfo.fillAllHoles();
		mVertexAlter = smartNew(VertexAlter);
		mVertexAlter->resize(this->n_vertices());
		initProperty();
	}
	~WatertightMesh(void);

	Skeleton_ getSkeleton() const { return mSkeleton; }

	void setSkeleton(Skeleton_ val) { mSkeleton = val; }	

	Segment_ getSegment() const { return mSegment; }

	void setSegment(Segment_ val) { mSegment = val; }

	/* 返回测地源点, 默认为最高点 */
	size_t getGeodesicSource(){
		Mesh::VertexHandle vh = *vertices_begin();
		double maxY = (double)_I32_MIN;
		for(Mesh::VertexIter vIt = vertices_begin(); vIt != vertices_end(); vIt++){
			double curY = this->point(*vIt).values_[1];
			if( curY > maxY){
				maxY = curY;
				vh = *vIt;
			}
		}
		return vh.idx();
	}

	void alterVertex(size_t index, Vec3d delta){
		mVertexAlter->add(index, delta);
	}

	void alterOriginalMesh();

	IntProperty_ getVertexPropertySKN() const { return mVertexPropertySKN; }
	void setVertexPropertySKN(IntProperty_ val) { mVertexPropertySKN = val; }

	DoubleProperty_ getVertexPropertyGeoDis() const { return mVertexPropertyGeoDis; }
	void setVertexPropertyGeoDis(DoubleProperty_ val) { mVertexPropertyGeoDis = val; }

	/* 获取网格顶点对应的骨骼节点 */
	size_t getCorrSkeletonNode(size_t vertex){
		return mVertexPropertySKN->get(vertex);
	}

	void dumpSkeLinkMesh()
	{
#ifdef _DEBUG_
		std::string path = "../data/skeLinkMesh/";
		for(size_t i = 0; i < mSkeleton->nodeCount(); i++){
			char back[8];
			sprintf(back, "%d.cg", i);
			std::ofstream out = std::ofstream(path + getName() + back);
			int edgeCount = mSkeleton->nodeAt(i)->correspondanceIndices.size();
			out << "# D:3 NV:" << n_vertices()+1
				<< " NE:" << edgeCount << "\n";
			for(Mesh::VertexIter vi = vertices_begin(); vi != vertices_end();
				vi++){
					Vec3d p = point(*vi);
					out << "v " << p.values_[0] << " " << p.values_[1] << " " << p.values_[2] << "\n";
			}
			SkeletonNode_ skn = mSkeleton->nodeAt(i);
			out << "v " << skn->point.values_[0] << " " << skn->point.values_[1] << " "
				<< skn->point.values_[2] << "\n";
			std::vector<size_t> cors = skn->correspondanceIndices;
			for(size_t k = 0; k < cors.size(); k++){
				out << "e " << cors.at(k)+1 << " " << n_vertices()+1 << "\n";
			}
			out.close();
		}
#endif
	}
};
S_PTR(WatertightMesh);

