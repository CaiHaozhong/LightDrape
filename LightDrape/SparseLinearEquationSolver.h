#include <vector>
#include <tuple>
#include <Eigen/Sparse>
//#include <Eigen/Dense>
#include <Eigen/SparseCholesky>
class SparseLinearEquationSolver
{
public:
	/************************************************************************/
	/* �����leftMatrix��һ��ϡ�����
	/* std::vector<std::pair<int, double> >Ϊһ��
	/* pair�洢һ��λ�ö�Ӧ��ֵ
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