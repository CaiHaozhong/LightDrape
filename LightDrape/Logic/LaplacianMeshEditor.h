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

	/* 设置新模型（尖点的模型）的顶点 */
	void setDeltaVertices(std::vector<LaplacianMeshEditorVertex> *deltaVertices);

	/* 原模型的面 */
	void setOriginFaceList(std::vector<LaplacianMeshEditorFace> *originFaceList);

	/* 原模型的顶点 */
	void setOriginVertexList(std::vector<LaplacianMeshEditorVertex> *originVertexList);

	/* 计算得到结果 */
	std::vector<LaplacianMeshEditorVertex>* compute();

	/* 当调用无参构造函数的时候，需手动构造Aprime  */
	void constructAprime();

	/* compute() 之前先分解方程左边的矩阵 */
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

	/* 每个顶点的度数 */
	Eigen::VectorXd degreeMatrix;

	unsigned int mMeshVertexCount;

	Eigen::VectorXd b;

	Eigen::SimplicialLDLT<Eigen::SparseMatrix<double> > solver;

	bool mHasFactorize;
};

