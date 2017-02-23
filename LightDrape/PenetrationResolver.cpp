#include "PenetrationResolver.h"
#include "Mesh.h"
#include "SparseLinearEquationSolver.h"
#include "KNNSHelper.h"
#include <fstream>

PenetrationResolver::PenetrationResolver(void)
{
	mHuman = nullptr;
	mGarment = nullptr;
	mEPS = -0.5;
}

PenetrationResolver::PenetrationResolver( Mesh_ garment )
{
	mGarment = garment;
	mHuman = nullptr;
	mEPS = -0.5;
}


PenetrationResolver::~PenetrationResolver(void)
{
}

void PenetrationResolver::setHumanMesh( Mesh_ human )
{
	mHuman = human;	
}


void PenetrationResolver::setGarmentMesh( Mesh_ garment )
{
	mGarment = garment;
}

/* Ax = b, x is transpose of (x0 ... xn y0 ... yn z0 ... zn) */
bool PenetrationResolver::resolve()
{
	if(mGarment == nullptr || mHuman == nullptr){
		PRINT_ERROR("In PenetrationResolver::resolve, mGarment is null or mHuman is null");
		return false;
	}
	int garVerCount = mGarment->n_vertices();
	std::vector<double> rightB;
	std::vector< std::vector< std::pair<int, double> > > leftMatrix;//稀疏矩阵，pair为下标和值
	std::vector<bool> penetrationTest;// = new bool[garVerCount];
	//memset(penetrationTest, 0, garVerCount);
	std::vector<int> nearestHumanVertex;
	computeNNSHumanVertex(nearestHumanVertex);
	computePenetrationVertices(nearestHumanVertex, penetrationTest);
	/** 构造左矩阵和右向量 **/
	for(int i = 0; i < garVerCount; i++){
		auto curVertex = mGarment->point(OpenMesh::VertexHandle(i)).values_;
		/** 一行 **/
		std::vector< std::pair<int, double> > row;	
		if(penetrationTest[i] == false){						
			for (int j = 0; j < 3; j++){
				row.clear();
				row.push_back(std::make_pair(i+j*garVerCount,1));
				leftMatrix.push_back(row);
				rightB.push_back(curVertex[j]);
			}
		}
		else{
			auto curNNSHumanVertexIndex = nearestHumanVertex.at(i);
			auto curNNSHumanVertex = mHuman->point(OpenMesh::VertexHandle(curNNSHumanVertexIndex)).values_;
			auto curNNSHumanNormal = mHuman->normal(OpenMesh::VertexHandle(curNNSHumanVertexIndex)).values_;
			row.clear();
			row.push_back(std::make_pair(i,curNNSHumanNormal[0]));
			row.push_back(std::make_pair(i+garVerCount,curNNSHumanNormal[1]));
			row.push_back(std::make_pair(i+2*garVerCount,curNNSHumanNormal[2]));
			leftMatrix.push_back(row);
			rightB.push_back(curNNSHumanNormal[0]*curNNSHumanVertex[0]
			+curNNSHumanNormal[1]*curNNSHumanVertex[1]
			+curNNSHumanNormal[2]*curNNSHumanVertex[2]
			-mEPS);
		}
	}
	/** 加入约束，一个顶点的形变与它周围顶点的平均形变相似**/
	std::vector< std::vector<size_t> > adjList;
	computeAdjList(mGarment, adjList);
	for (int i = 0; i < garVerCount; i++){
		if(!penetrationTest[i])
			continue;
		std::vector< std::pair<int, double> > row;
		std::vector<size_t>& adjs = adjList[i];
		std::vector<size_t> penetratedAdj;
		for(auto it = adjs.begin(); it != adjs.end(); it++){
			// 			if(penetrationTest.at(it->idx()))
			// 			{
			// 				penetratedAdj.push_back(*it);
			// 			}
			penetratedAdj.push_back(*it);
		}
		int penetratedAdjSize = penetratedAdj.size();
		for (int k = 0; k < 3; k++){
			row.clear();
			row.push_back(std::make_pair(i+k*garVerCount,1));			
			for (int j = 0; j < penetratedAdjSize; j++)
			{
				row.push_back(std::make_pair(penetratedAdj.at(j)+k*garVerCount, -1.0/penetratedAdjSize));
			}
			leftMatrix.push_back(row);
		}
		auto curVertex = mGarment->point(Mesh::VertexHandle(i)).values_;
		for (int k = 0; k < 3; k++)
		{
			double adjSum = 0;
			for (int j = 0; j < penetratedAdjSize; j++)
			{
				adjSum += mGarment->point(Mesh::VertexHandle(penetratedAdj.at(j))).values_[k];
			}
			rightB.push_back(curVertex[k]-1.0/penetratedAdjSize*adjSum);
		}
	}
	std::vector<double> ret;
	ret.reserve(3*garVerCount);
	bool isSuccess = SparseLinearEquationSolver().solve(leftMatrix,rightB, 3*garVerCount, ret);
/*	delete [] penetrationTest;*/
	if(isSuccess)
	{
		auto it_end = mGarment->vertices_end();
		int index = 0;
		for(Mesh::VertexIter v_it = mGarment->vertices_begin(); v_it != it_end; v_it++)
		{
			Mesh::Point& p = mGarment->point(*v_it);
			p.values_[0] = ret[index];
			p.values_[1] = ret[index+garVerCount];
			p.values_[2] = ret[index+garVerCount*2];
			index += 1;
		}
		return true;
	}
	else
		return false;
}

void PenetrationResolver::computeNNSHumanVertex( std::vector<int>& nearestHumanVertex )
{
	if(mGarment == nullptr || mHuman == nullptr){
		PRINT_ERROR("In PenetrationResolver::computeNNSHumanVertex, mGarment is null or mHuman is null");
		return;
	}
	size_t garVerCount = mGarment->n_vertices();
	nearestHumanVertex.clear();
	nearestHumanVertex.resize(garVerCount, -1);
	std::vector<Vec3d> pointCloud;
	pointCloud.reserve(mHuman->n_vertices());
	for(Mesh::VertexIter it = mHuman->vertices_begin(); it != mHuman->vertices_end(); it++){
		pointCloud.push_back(mHuman->point(*it));
	}
	KNNSHelper knnsHelper(pointCloud);	
	int cur = 0;
	KNNSHelper::Result result;
	for(Mesh::VertexIter it = mGarment->vertices_begin(); it != mGarment->vertices_end(); it++){
		Vec3d& query = mGarment->point(*it);		
		knnsHelper.singleNeighborSearch(query, result);
		nearestHumanVertex[cur++] = result.index;
	}

	std::ofstream out("../data/nnmap.cg");
	out << "# D:3 NV:" << garVerCount*2
		<< " NE:" << garVerCount << "\n";
	for(size_t i = 0; i < garVerCount; i++){
		auto& p = mGarment->point(Mesh::VertexHandle(i)).values_;
		out << "v " << p[0] << " " << p[1] << " "
			<< p[2] << "\n";
		size_t n = nearestHumanVertex[i];
		auto& hp = mHuman->point(Mesh::VertexHandle(n)).values_;
		out << "v " << hp[0] << " " << hp[1] << " "
			<< hp[2] << "\n";
	}
	for(size_t i = 0; i < garVerCount; i++){
		out << "e " << 2*i+1 << " " << 2*i+2 << "\n";
	}
	out.close();
}

void PenetrationResolver::computePenetrationVertices( const std::vector<int>& nearestHumanVertex, std::vector<bool>& penetrationTest )
{
	penetrationTest.resize(mGarment->n_vertices(), false);
	if(mGarment == nullptr || mHuman == nullptr){
		PRINT_ERROR("In PenetrationResolver::computePenetrationVertices, mGarment is null or mHuman is null");
		return;
	}
	int curClothVertexIndex = 0;
	for(Mesh::VertexIter it = mGarment->vertices_begin(); it != mGarment->vertices_end(); it++){
		Vec3d& garVer = mGarment->point(*it);
		int humIndex = nearestHumanVertex[curClothVertexIndex];
		if(humIndex == -1){
			PRINT_ERROR("In PenetrationResolver::computePenetrationVertices, humIndex == -1");
			continue;
		}
		Vec3d& nearestHumanVer = mHuman->point(Mesh::VertexHandle(humIndex));
		Mesh::Normal humNormal = mHuman->normal(Mesh::VertexHandle(humIndex));
		double dotProduct = (garVer-nearestHumanVer)|humNormal;
		if(dotProduct < 0)
			penetrationTest[curClothVertexIndex] = true;
		curClothVertexIndex += 1;
	}

	std::vector<size_t> isPene;
	for(size_t i = 0; i < mGarment->n_vertices(); i++){
		if(penetrationTest[i]){
			isPene.push_back(i);
		}
	}
	std::ofstream out("../data/penetration.cg");
	out << "# D:3 NV:" << isPene.size()
		<< " NE:" << 0 << "\n";
	for(size_t i = 0; i < isPene.size(); i++){
		auto& p = mGarment->point(Mesh::VertexHandle(isPene[i])).values_;
		out << "v " << p[0] << " " << p[1] << " "
			<< p[2] << "\n";
	}
	out.close();
}

void PenetrationResolver::setGarHumDis( double eps )
{
	mEPS = -eps;
}

void PenetrationResolver::computeAdjList( Mesh_ mesh, std::vector< std::vector<size_t> >& adjList )
{
	if(mesh == nullptr)
		return;

	int cur = 0;
	adjList.resize(mesh->n_vertices(), std::vector<size_t>());
	for(Mesh::VertexIter it = mesh->vertices_begin(); it != mesh->vertices_end(); it++){
		for(Mesh::ConstVertexVertexIter vvIt = mesh->cvv_begin(*it); vvIt.is_valid(); vvIt++){
			adjList[cur].push_back(vvIt->idx());
		}
		cur += 1;
	}
}
