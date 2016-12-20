#include "SparseLinearEquationSolver.h"
#include <iostream>
bool SparseLinearEquationSolver::solve( const std::vector< std::vector<std::pair<int, double> > >& leftMatrix, const std::vector<double>& rightB, int leftMatrixWidth, std::vector<double>& ret )
{
	int leftMatrixHeight = leftMatrix.size();
	X.resize(leftMatrixHeight, leftMatrixWidth);
	printf("Matrix Size: %d,%d\n",leftMatrixHeight, leftMatrixWidth);
	for (int i = 0; i < leftMatrixHeight; i++)
	{
		std::vector<std::pair<int, double> > r = leftMatrix.at(i);
		for (int j = 0; j < r.size(); j++)
		{
			std::pair<int, double> item = r.at(j);
			X.insert(i,item.first) = item.second;
		}
	}
	b.resize(leftMatrixHeight);
	for (int i = 0; i < leftMatrixHeight; i++)
	{
		b(i) = rightB.at(i);
	}
	Eigen::SparseMatrix<double> XT = Eigen::SparseMatrix<double>(X.transpose());
	Eigen::SparseMatrix<double> XTxX = XT * X;
	Eigen::VectorXd XTxb = XT * b;
	solver.compute(XTxX);
	if(solver.info()!= Eigen::Success) {
		std::string errorMsg = std::string("SparseCholesky Decomposition error,'solver.compute(AprimeTxAprime)'") 
			+ " at file: " + __FILE__ + ", line: ";
		char buf[20];
		sprintf_s(buf,"%d\n",__LINE__);
		errorMsg += std::string(buf);
		fprintf(stderr,errorMsg.c_str());
		solver.dumpMemory(std::cout);
		return false;
	}	
	Eigen::VectorXd v_p = solver.solve(XTxb);
	if(solver.info()!= Eigen::Success) {
		std::string errorMsg = std::string("solving error,'Eigen::VectorXd v_p = solver.solve(AprimeTxb);'") 
			+ " at file: " + __FILE__ + ", line: ";
		char buf[20];
		sprintf_s(buf,"%d",__LINE__);
		errorMsg += std::string(buf);
		fprintf(stderr, errorMsg.c_str());
		
		return false;
	}
	ret.resize(leftMatrixWidth);
	for (int i = 0; i < leftMatrixWidth; i++)
	{
		ret[i] = v_p(i);
	}
	return true;
}

