#pragma once
#include <vector>
#include "WatertightMesh.h"
/* edge 是该Node所在的边的下标
 * start_vertex和factor用来计算
 * 该Node的位置
 * point(start_vertex) + factor * (point(end_vertex)-point(start_vertex))
 */
class LevelNode{
public:
	size_t edge;
	size_t start_vertex;
	double factor;

	/* 获取和该Node位置最接近的网格顶点的下标 */
	size_t getNearestVertex(Mesh_ mesh){
		if(factor < 0.5){
			return start_vertex;
		}
		else{
			return getToVertexIndex(mesh);
		}
	}

	/* 该节点的确切位置
	 * point(start_vertex) + factor * (point(end_vertex)-point(start_vertex))
	 */
	Vec3d exactlyPoint(Mesh_ mesh){
		Mesh::VertexHandle start_handle = Mesh::VertexHandle(start_vertex);
		Mesh::VertexHandle end_handle = Mesh::VertexHandle(getToVertexIndex(mesh));
		Vec3d& startPoint = mesh->point(start_handle);
		Vec3d ret =  startPoint + factor * (mesh->point(end_handle)- startPoint);
		return ret;
	}

	/* 计算与start_vertex对应的终点下标 */
	size_t getToVertexIndex(Mesh_ mesh){
		OpenMesh::EdgeHandle edgeHandle = OpenMesh::EdgeHandle(edge);
		if(mesh->is_boundary(edgeHandle)){
			PRINTLN("Error! In LevelNode. Watertight mesh should not has boundary edge.");
			return start_vertex;
		}
		OpenMesh::HalfedgeHandle heHandle = mesh->halfedge_handle(edgeHandle, 0);
		size_t ret = mesh->to_vertex_handle(heHandle).idx();
		if(ret == start_vertex)
			ret = mesh->from_vertex_handle(heHandle).idx();
		return ret;
	}
};
S_PTR(LevelNode);

/* 一个独立的圈，这个圈里的节点与测地源点的测地距相等 */
class LevelCircle{
public:
	LevelCircle(){
		isCenterXValid = false;
	}
	std::vector<LevelNode_> levelNodes;
	double mCenterX;
	bool isCenterXValid;
	void addNode(LevelNode_ node){
		levelNodes.push_back(node);
	}
	/* 判断新的LevelNode是否在这个圈里面
	 * 即，与这个圈里的任意一个节点相邻
	 */
	bool isInThisCircle(LevelNode_ newNode, Mesh_ mesh){
		Mesh::EdgeHandle newEdge = Mesh::EdgeHandle(newNode->edge);
		for(size_t i = 0; i < levelNodes.size(); i++){
			if(isNeighbor(newEdge, Mesh::EdgeHandle(levelNodes[i]->edge), mesh)){
				return true;
			}
		}
		return false;
	}

	/* 计算这个Circle的中心点的X坐标
	 * TODO: 也可以用骨骼节点来代替，可能更好
	 */
	void computeCenter(Mesh_ mesh){
		double minX = std::numeric_limits<double>::max();
		double maxX = std::numeric_limits<double>::min();
		for(size_t i = 0; i < levelNodes.size(); i++){
			Vec3d exactPoint = levelNodes[i]->exactlyPoint(mesh);
			double epX = exactPoint.values_[0];
			if(epX > maxX){
				maxX = epX;
			}
			if(epX < minX){
				minX = epX;
			}
		}
		mCenterX = 0.5 * (maxX + minX);
		isCenterXValid = true;
	}

	double getCenterX(Mesh_ mesh){
		if(isCenterXValid == false){
			computeCenter(mesh);
		}
		return mCenterX;
	}
private:
	/* 判断两条边是否相邻 */
	bool isNeighbor(Mesh::EdgeHandle a, Mesh::EdgeHandle b, Mesh_ mesh){		
		Mesh::HalfedgeHandle halfEdge = mesh->halfedge_handle(a, 0);
		Mesh::VertexHandle fromV_a = mesh->from_vertex_handle(halfEdge);
		Mesh::VertexHandle toV_a = mesh->to_vertex_handle(halfEdge);
		for(Mesh::VertexEdgeIter ve_it = mesh->ve_begin(fromV_a); ve_it.is_valid(); ve_it++){
			if(b == *ve_it){
				return true;
			}
		}
		for(Mesh::VertexEdgeIter ve_it = mesh->ve_begin(toV_a); ve_it.is_valid; ve_it++){
			if(b == *ve_it){
				return true;
			}
		}
		return false;
	}
};
S_PTR(LevelCircle);
class LevelSet{
private:
	WatertightMesh_ mMesh;
	std::vector<LevelCircle_> mCircles;
	std::vector<LevelNode_> mRawNodes;
public:
	LevelSet(){}
	~LevelSet(){}
	void init(WatertightMesh_ mesh, std::vector<LevelNode_>& nodes){
		mMesh = mesh;
		mRawNodes.resize(nodes.size());
		std::copy(nodes.begin(), nodes.end(), mRawNodes.begin());
		classify();		
	}

	size_t getCount(){
		return mCircles.size();
	}

	LevelCircle_ getCircle(size_t categoryIndex){
		if(categoryIndex >=0 && categoryIndex < mCircles.size())
			return mCircles.at(categoryIndex);
		else{
			PRINTLN("ERROR! In LevelSet. Index out of boundary.");
			return nullptr;
		}
	}

private:
	/* 对该LevelSet上的点进行分类
	 * 比如分成左手、躯干、右手三个环
	 */
	void classify(){
		while(mRawNodes.empty() == false){			
			LevelNode_ ln = mRawNodes.back();			
			bool belongToOneCircle = false;
			for(size_t i = 0; i < mCircles.size(); i++){
				LevelCircle_ lc = mCircles[i];
				if(lc->isInThisCircle(ln, mMesh)){
					lc->addNode(ln);
					belongToOneCircle = true;					
				}
			}
			if(!belongToOneCircle){
				LevelCircle_ newCircle = smartNew(LevelCircle);
				newCircle->addNode(ln);
				mCircles.push_back(newCircle);
			}
			mRawNodes.pop_back();
		}
	}
};