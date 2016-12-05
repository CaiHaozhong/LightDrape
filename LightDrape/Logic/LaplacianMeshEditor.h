#pragma once
#include <vector>
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <Eigen/SparseCholesky>
#include <OpenMesh/Core/Geometry/VectorT.hh>
class LaplacianMeshEditorFace
{
public:
	/* Vertex indices */
	std::vector<unsigned int> v;

	/* Texture Coordinate indices */
	std::vector<unsigned int> vt;

	/* Normal indices */
	std::vector<unsigned int> vn;

	int getVertexNum(){ return v.size(); }
};
class LaplacianMeshEditorVertex
{
public:
	double x,y,z;
	LaplacianMeshEditorVertex(double _x, double _y, double _z):x(_x),y(_y),z(_z){}
	LaplacianMeshEditorVertex(){}
	OpenMesh::Vec3d toOpenMeshVector()
	{
		return OpenMesh::Vec3d(x,y,z);
	}
};
class LaplacianMeshEditor
{
public:
	LaplacianMeshEditor(void);
	~LaplacianMeshEditor(void);

	LaplacianMeshEditor(std::vector<LaplacianMeshEditorFace> *originFaceList, std::vector<LaplacianMeshEditorVertex> *originVertexList);

	/* ������ģ�ͣ�����ģ�ͣ��Ķ��� */
	void setDeltaVertices(std::vector<LaplacianMeshEditorVertex> *deltaVertices);

	/* ԭģ�͵��� */
	void setOriginFaceList(std::vector<LaplacianMeshEditorFace> *originFaceList);

	/* ԭģ�͵Ķ��� */
	void setOriginVertexList(std::vector<LaplacianMeshEditorVertex> *originVertexList);

	/* ����õ���� */
	std::vector<LaplacianMeshEditorVertex>* compute();

	/* �������޲ι��캯����ʱ�����ֶ�����Aprime  */
	void constructAprime();

	/* compute() ֮ǰ�ȷֽⷽ����ߵľ��� */
	void factorizeAprimeTxAprime();
private:
	void _initAdjacentMatrix();

	void _computeLaplacianOperator(Eigen::SparseMatrix<double>& laplacianOperator);

	void _getAdjVertex(int v, std::vector<int>& adj);	

	void _constructMatrix(const LaplacianMeshEditorVertex& v, Eigen::Matrix<double,3,7>& m);
private:
	std::vector<LaplacianMeshEditorFace> *mOriginFaceList;

	std::vector<LaplacianMeshEditorVertex> *mOriginVertexList;

	std::vector<LaplacianMeshEditorVertex> *mDeltaVertices;

	Eigen::SparseMatrix<double> Aprime;

	/* Transpose of Aprime */
	Eigen::SparseMatrix<double> AprimeT;

	Eigen::SparseMatrix<double> AprimeTxAprime;

	Eigen::SparseMatrix<double> adjacentMatrix;

	/* ÿ������Ķ��� */
	Eigen::VectorXd degreeMatrix;

	unsigned int mMeshVertexCount;

	Eigen::VectorXd b;

	Eigen::SimplicialLDLT<Eigen::SparseMatrix<double> > solver;

	bool mHasFactorize;
};

