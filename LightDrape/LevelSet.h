#pragma once
#include <vector>
#include "WatertightMesh.h"
/* edge �Ǹ�Node���ڵıߵ��±�
 * start_vertex��factor��������
 * ��Node��λ��
 * point(start_vertex) + factor * (point(end_vertex)-point(start_vertex))
 */
class LevelNode{
public:
	size_t edge;
	size_t start_vertex;
	double factor;

	/* ��ȡ�͸�Nodeλ����ӽ������񶥵���±� */
	size_t getNearestVertex(Mesh_ mesh){
		if(factor < 0.5){
			return start_vertex;
		}
		else{
			return getToVertexIndex(mesh);
		}
	}

	/* �ýڵ��ȷ��λ��
	 * point(start_vertex) + factor * (point(end_vertex)-point(start_vertex))
	 */
	Vec3d exactlyPoint(Mesh_ mesh){
		Mesh::VertexHandle start_handle = Mesh::VertexHandle(start_vertex);
		Mesh::VertexHandle end_handle = Mesh::VertexHandle(getToVertexIndex(mesh));
		Vec3d& startPoint = mesh->point(start_handle);
		Vec3d ret =  startPoint + factor * (mesh->point(end_handle)- startPoint);
		return ret;
	}

	/* ������start_vertex��Ӧ���յ��±� */
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

/* һ��������Ȧ�����Ȧ��Ľڵ�����Դ��Ĳ�ؾ���� */
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
	/* �ж��µ�LevelNode�Ƿ������Ȧ����
	 * ���������Ȧ�������һ���ڵ�����
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

	/* �������Circle�����ĵ��X����
	 * TODO: Ҳ�����ù����ڵ������棬���ܸ���
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
	/* �ж��������Ƿ����� */
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
	/* �Ը�LevelSet�ϵĵ���з���
	 * ����ֳ����֡����ɡ�����������
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