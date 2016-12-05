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

	/* ���У�����Ĳ���Ϊ�·��Ĺ������Ѿ���������룬���Ҽ����˽ڵ��deltaֵ
	   �ú����ı�globalMeshContainer�е��·����� */
	void deformPose(const Skeleton& skeleton);

	bool resolvePenetration(const Mesh& humanMesh, double eps);

	/* A single step */
	void physicalSimulate(double dt);

	void setPhysicalConstrainer(PhysicalConstrainer* physicalConstrainer);
private:
	/** �����·�����δ�ƶ��Ķ���ļ��ϣ���ԭʼ���� **/
	void computeDeltaVertices( const Skeleton& skeleton, std::vector<LaplacianMeshEditorVertex>& newVertexList);
	
	/** ����nnsHumanVertex��Ϊ������±�Ϊ�·�����������ֵΪ������·�������������嶥������ **/
	void computeNNSHumanVertex(const Mesh& clothMesh, const Mesh& humanMesh, std::vector<int>& nnsHumanVertex);

	/** penetrationVerticesΪ������±�Ϊ�·������������д�͸��Ϊtrue������Ϊfalse **/
	void computePenetrationVertices(const Mesh& clothMesh, const Mesh& humanMesh, const std::vector<int>& nnsHumanVertex, std::vector<bool>& penetrationTest);

	/** ����ÿ��������ڽӶ��� **/
	void computeAdjList(const Mesh& clothMesh, std::vector< std::vector<Mesh::VertexHandle> >& adjList);



	PhysicalConstrainer* mPhysicalConstrainer;
};

