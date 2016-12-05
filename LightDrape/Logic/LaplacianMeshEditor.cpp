#include "LaplacianMeshEditor.h"
#include <time.h>
#include <stdio.h>
#include <Eigen/SparseQR>
clock_t clock_start;
clock_t clock_end;
void clockStart(){clock_start = clock();}
void clockEnd(){clock_end = clock();}
void printTime(const char* str){fprintf(stdout,"%s:%dms\n",str,clock_end-clock_start);}
LaplacianMeshEditor::LaplacianMeshEditor(void)
{
	this->mOriginFaceList = nullptr;
	this->mOriginVertexList = nullptr;
	this->mDeltaVertices = nullptr;
	this->mMeshVertexCount = 0;
}

LaplacianMeshEditor::LaplacianMeshEditor( std::vector<LaplacianMeshEditorFace> *originFaceList, std::vector<LaplacianMeshEditorVertex> *originVertexList )
{
	this->mOriginFaceList = originFaceList;
	this->mOriginVertexList = originVertexList;
	this->mDeltaVertices = nullptr;
	this->mMeshVertexCount = originVertexList->size();

	constructAprime();
}


LaplacianMeshEditor::~LaplacianMeshEditor(void)
{
}

/*
遍历所有的面，初始化邻接矩阵
*/
void LaplacianMeshEditor::_initAdjacentMatrix()
{
	if(mMeshVertexCount == 0)
		return;
	
	adjacentMatrix.resize(mMeshVertexCount,mMeshVertexCount);	
	adjacentMatrix.reserve(Eigen::VectorXi::Constant(mMeshVertexCount,10));
	degreeMatrix.resize(mMeshVertexCount);
	degreeMatrix.setZero();
	
	for (int i = 0; i < mOriginFaceList->size(); i++)
	{
		LaplacianMeshEditorFace face = mOriginFaceList->at(i);
		int faceVertexCount = face.getVertexNum();
		for (int j = 0; j < faceVertexCount; j++)
		{
			int verIndex = face.v.at(j);
			int adj = (j+1)%faceVertexCount;
			Eigen::SparseMatrix<double>::Scalar val = adjacentMatrix.coeff(verIndex,face.v.at(adj));			
			if(fabs(val-1) > 0.001f)
			{
				adjacentMatrix.insert(verIndex,face.v.at(adj)) = 1;
				degreeMatrix(verIndex) += 1;
			}
			adj = j - 1;
			if(adj < 0)
				adj += faceVertexCount;			
			val = adjacentMatrix.coeff(verIndex,face.v.at(adj));
			if(fabs(val-1) > 0.001f)
			{
				adjacentMatrix.insert(verIndex,face.v.at(adj)) = 1;
				degreeMatrix(verIndex) += 1;
			}
		}
	}
}

void LaplacianMeshEditor::_computeLaplacianOperator( Eigen::SparseMatrix<double>& laplacianOperator )
{

	laplacianOperator.resize(mMeshVertexCount,mMeshVertexCount);
	laplacianOperator.reserve(Eigen::VectorXi::Constant(mMeshVertexCount,10));
	for (int i = 0; i < mMeshVertexCount; i++)
	{
		/* 如果第i个点没有邻接点，即它是一个孤立的点，那么它的laplacian坐标为0 */
		if( adjacentMatrix.innerVector(i).nonZeros() == 0)
		{
			laplacianOperator.insert(i,i) = 0;
			continue;
		}
		laplacianOperator.insert(i,i) = 1;
#ifdef MY_DEBUG
		int adjCount = 0;
#endif		
		for (Eigen::SparseMatrix<double>::InnerIterator it(adjacentMatrix,i); it; it++)
		{
			if(i != it.row())
			{
				laplacianOperator.insert(i,it.row()) = -1/degreeMatrix(i);				
#ifdef MY_DEBUG
				adjCount++;
				if(adjCount >= 10)
					printf("InnerVector size should expand! CurrentMax:%d.\n",adjCount);
#endif
			}
		}
	}
}

/* 构造矩阵Aprime */
void LaplacianMeshEditor::constructAprime()
{
	clockStart();

	_initAdjacentMatrix();	

	Eigen::SparseMatrix<double> laplacianOperator;
	_computeLaplacianOperator(laplacianOperator);

	Eigen::Matrix<double,Eigen::Dynamic,3> verInput(mMeshVertexCount,3);	
	for (int i = 0; i < mMeshVertexCount; i++)
	{
		LaplacianMeshEditorVertex v = mOriginVertexList->at(i);
		verInput(i,0) = v.x; verInput(i,1) = v.y; verInput(i,2) = v.z;
	}

	Eigen::Matrix<double,Eigen::Dynamic,3> deltaInput = laplacianOperator * verInput;

	Aprime.resize(6*mMeshVertexCount,3*mMeshVertexCount);	
	Aprime.reserve(Eigen::VectorXi::Constant(3*mMeshVertexCount,40));

	/* 操作 Aprime */
	for (int i = 0; i < mMeshVertexCount; i++)
	{
		/* 生成矩阵 A*/
		LaplacianMeshEditorVertex v = mOriginVertexList->at(i);
		std::vector<LaplacianMeshEditorVertex> vertexInA;
		vertexInA.push_back(v);
		std::vector<int> adjIndex;
		_getAdjVertex(i,adjIndex);
		for (int j = 0; j < adjIndex.size(); j++)
		{
			vertexInA.push_back(mOriginVertexList->at(adjIndex.at(j)));
		}

		Eigen::Matrix<double,Eigen::Dynamic,7> matrixA;
		matrixA.resize(vertexInA.size()*3,7);
		for (int j = 0; j < vertexInA.size(); j++)
		{
			Eigen::Matrix<double,3,7> m;
			_constructMatrix(vertexInA.at(j),m);
			int r = matrixA.rows();			
			matrixA.middleRows<3>(j*3) = m;
		}		
		Eigen::Matrix<double,7,Eigen::Dynamic> pinvA;

		/* 当该顶点是孤立的的时候，transposeA*matrixA可能是奇异的，因此pinv就会出现-1.#IND*/
		if(adjIndex.size() == 0)
		{
			pinvA.resize(7,3);
			pinvA.setZero();
		}
		else
		{
			Eigen::Matrix<double,7,Eigen::Dynamic> transposeA = matrixA.transpose();
			pinvA = (transposeA * matrixA).inverse()*transposeA;
		}

		Eigen::Matrix<double,1,Eigen::Dynamic> s = pinvA.row(0);
		Eigen::Matrix<double,1,Eigen::Dynamic> h1 = pinvA.row(1);
		Eigen::Matrix<double,1,Eigen::Dynamic> h2 = pinvA.row(2);
		Eigen::Matrix<double,1,Eigen::Dynamic> h3 = pinvA.row(3);

		Eigen::Matrix<double,1,Eigen::Dynamic> TDeltaX = s*deltaInput(i,0)-h3*deltaInput(i,1)+h2*deltaInput(i,2);
		Eigen::Matrix<double,1,Eigen::Dynamic> TDeltaY = h3*deltaInput(i,0)+s*deltaInput(i,1)-h1*deltaInput(i,2);
		Eigen::Matrix<double,1,Eigen::Dynamic> TDeltaZ = -h2*deltaInput(i,0)-h1*deltaInput(i,1)+s*deltaInput(i,2);

		Eigen::Matrix<double,3,Eigen::Dynamic> TDelta;
		TDelta.resize(3,TDeltaX.cols());
		TDelta.row(0) = TDeltaX; TDelta.row(1) = TDeltaY; TDelta.row(2) = TDeltaZ;

		for (int j = 0; j < 3; j++)
		{
			int opRow = j*mMeshVertexCount+i;
			Aprime.insert(opRow,i) = j == 0 ? -TDelta(j,0) + laplacianOperator.coeff(i,i) : -TDelta(j,0);
			Aprime.insert(opRow,i+mMeshVertexCount) = j == 1 ? -TDelta(j,1) + laplacianOperator.coeff(i,i) : -TDelta(j,1);
			Aprime.insert(opRow,i+2*mMeshVertexCount) = j == 2 ? -TDelta(j,2) + laplacianOperator.coeff(i,i) : -TDelta(j,2);
			int curTDeltaCol = 3;
			for (int a = 0; a < adjIndex.size(); a++)
			{
				int adj = adjIndex.at(a);
				Aprime.insert(opRow,adj) = j == 0 ? -TDelta(j,curTDeltaCol++) + laplacianOperator.coeff(i,adj) : -TDelta(j,curTDeltaCol++);
				Aprime.insert(opRow,adj+mMeshVertexCount) = j == 1 ? -TDelta(j,curTDeltaCol++) + laplacianOperator.coeff(i,adj) :  -TDelta(j,curTDeltaCol++);
				Aprime.insert(opRow,adj+2*mMeshVertexCount) = j == 2 ? -TDelta(j,curTDeltaCol++) + laplacianOperator.coeff(i,adj) : -TDelta(j,curTDeltaCol++);
			}
		}		
	}
	int mMeshVertexCount_X3 = 3*mMeshVertexCount;
	int offset = 0;
	for(int j = 0; j < mMeshVertexCount_X3; j+=3)
	{
		Aprime.insert(mMeshVertexCount_X3+j,offset) = 1;
		Aprime.insert(mMeshVertexCount_X3+j+1,offset+mMeshVertexCount) = 1;
		Aprime.insert(mMeshVertexCount_X3+j+2,offset+2*mMeshVertexCount) = 1;
		offset++;
	}
	Aprime.makeCompressed();

	AprimeT = Eigen::SparseMatrix<double>(Aprime.transpose());
	AprimeTxAprime = AprimeT*Aprime;
	mHasFactorize = false;
	clockEnd();
	printTime("Construct");
}

/* 获取下标为v的顶点的邻接顶点，以下标的形式返回*/
void LaplacianMeshEditor::_getAdjVertex( int v, std::vector<int>& adj )
{
	for (Eigen::SparseMatrix<double>::InnerIterator it(adjacentMatrix,v); it; it++)
	{
		if(it.row() != v)
			adj.push_back(it.row());
	}
}

/* 根据一个顶点，构造矩阵A */
void LaplacianMeshEditor::_constructMatrix( const LaplacianMeshEditorVertex& v, Eigen::Matrix<double,3,7>& m )
{
	Eigen::Matrix<double,3,4> leftCols4;
	leftCols4 << v.x,	0,	    v.z,	-v.y,
		v.y,	-v.z,	0,		 v.x,
		v.z,	v.y,	-v.x,	 0;
	m.leftCols<4>() = leftCols4;
	m.rightCols<3>() = Eigen::Matrix<double,3,3>::Identity();
}

void LaplacianMeshEditor::setDeltaVertices( std::vector<LaplacianMeshEditorVertex> *deltaVertices )
{
	clockStart();

	this->mDeltaVertices = deltaVertices;
	if(this->mDeltaVertices->size() != mMeshVertexCount)
	{
		fprintf(stderr,"this->mCuspidalVertices->size() != mMeshVertexCount\n");
		return;
	}
	/* 构造方程右边的b */
	int mMeshVertexCount_X3 = 3*mMeshVertexCount;
	b.resize(mMeshVertexCount_X3*2);
	b.setZero();
		
	for (int j = 0; j < mMeshVertexCount; j++)
	{
		b(mMeshVertexCount_X3+3*j) = mDeltaVertices->at(j).x;
		b(mMeshVertexCount_X3+3*j+1) = mDeltaVertices->at(j).y;
		b(mMeshVertexCount_X3+3*j+2) = mDeltaVertices->at(j).z;
	}					

	clockEnd();
	printTime("setCuspidalVertices");	
}

void LaplacianMeshEditor::setOriginFaceList( std::vector<LaplacianMeshEditorFace> *originFaceList )
{
	this->mOriginFaceList = originFaceList;
}

void LaplacianMeshEditor::setOriginVertexList( std::vector<LaplacianMeshEditorVertex> *originVertexList )
{
	this->mOriginVertexList = originVertexList;
}


std::vector<LaplacianMeshEditorVertex>* LaplacianMeshEditor::compute()
{	

	/* Cholesky分解 */
	factorizeAprimeTxAprime();

	if(mHasFactorize == false)
	{
		fprintf(stderr,"You should use 'void factorizeAprimeTxAprime()' first.\n");
		return nullptr;
	}
	clockStart();
	Eigen::VectorXd AprimeTxb = AprimeT*b;
		
	Eigen::VectorXd v_p = solver.solve(AprimeTxb);

	if(solver.info()!= Eigen::Success) {
		fprintf(stderr,"solving error,'Eigen::VectorXd v_p = solver.solve(AprimeTxb);'");
		return nullptr;
	}
	clockEnd();
	printTime("solve");

	clockStart();
	std::vector<LaplacianMeshEditorVertex>* ret = new std::vector<LaplacianMeshEditorVertex>();
	int rows = v_p.rows();
	int verCount = rows/3;
	int verCountX2 = verCount*2;
	for (int i = 0; i < verCount; i++)
	{
		ret->push_back(LaplacianMeshEditorVertex(v_p(i),v_p(i+verCount),v_p(i+verCountX2)));		
	}
	clockEnd();
	printTime("return vertices");

	return ret;
}

/* SparseCholesky Decomposition */
void LaplacianMeshEditor::factorizeAprimeTxAprime()
{
	clockStart();
	solver.compute(AprimeTxAprime);

	if(solver.info()!= Eigen::Success) {
		fprintf(stderr,"SparseCholesky Decomposition error,'solver.compute(AprimeTxAprime)'");
		return;
	}
	clockEnd();
	printTime("factorize");

	mHasFactorize = true;
}
