#include <vector>
#include <tuple>
#include <Eigen/Sparse>
//#include <Eigen/Dense>
#include <Eigen/SparseCholesky>
class SparseLinearEquationSolver
{
public:
	/************************************************************************/
	/* 这里的leftMatrix是一个稀疏矩阵
	/* std::vector<std::pair<int, double> >为一行
	/* pair存储一个位置对应的值
	/************************************************************************/
	bool solve(const std::vector< std::vector<std::pair<int, double> > >& leftMatrix, 
		const std::vector<double>& rightB, 
		int leftMatrixWidth,
		std::vector<double>& ret);
private:
	Eigen::SparseMatrix<double> X;
	Eigen::VectorXd b;
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<double> > solver;
};