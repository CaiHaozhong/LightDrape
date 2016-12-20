#pragma once
#include "Mesh.h"
#include "Skeleton.h"
#include "Segment.h"
#include "VertexAlter.h"
#include "HoleInfo.h"
class WatertightMesh :
	public Mesh
{
private:
	Skeleton_ mSkeleton;
	Segment_ mSegment;

	Mesh_ mOriginalMesh;
	VertexAlter_ mVertexAlter;
public:
	WatertightMesh(Mesh_ mesh):Mesh(*mesh){		
		mOriginalMesh = mesh;
		HoleInfo holeInfo(this);
		holeInfo.getHoles();
		holeInfo.fillAllHoles();
		mVertexAlter = smartNew(VertexAlter);
		mVertexAlter->resize(this->n_vertices());
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
};

