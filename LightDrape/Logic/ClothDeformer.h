#pragma once
#include <vector>
#include "LaplacianMeshEditor.h"
#include "Skeleton.h"
#include "Mesh.h"
#include "PhysicalConstrainer.h"
class ClothDeformer
{
public:

	ClothDeformer();
	~ClothDeformer();

	/* 其中，输入的参数为衣服的骨骼，已经和网格对齐，并且计算了节点的delta值
	   该函数改变globalMeshContainer中的衣服网格 */
	void deformPose(const Skeleton& skeleton);

	bool resolvePenetration(const Mesh& humanMesh, double eps);

	/* A single step */
	void physicalSimulate(double dt);

	void setPhysicalConstrainer(PhysicalConstrainer* physicalConstrainer);
private:
	/** 传入衣服网格未移动的顶点的集合，即原始顶点 **/
	void computeDeltaVertices( const Skeleton& skeleton, std::vector<LaplacianMeshEditorVertex>& newVertexList);
	
	/** 参数nnsHumanVertex作为输出，下标为衣服顶点索引，值为距离该衣服顶点最近的人体顶点索引 **/
	void computeNNSHumanVertex(const Mesh& clothMesh, const Mesh& humanMesh, std::vector<int>& nnsHumanVertex);

	/** penetrationVertices为输出，下标为衣服顶点索引，有穿透则为true，否则为false **/
	void computePenetrationVertices(const Mesh& clothMesh, const Mesh& humanMesh, const std::vector<int>& nnsHumanVertex, std::vector<bool>& penetrationTest);

	/** 计算每个顶点的邻接顶点 **/
	void computeAdjList(const Mesh& clothMesh, std::vector< std::vector<Mesh::VertexHandle> >& adjList);



	PhysicalConstrainer* mPhysicalConstrainer;
};

