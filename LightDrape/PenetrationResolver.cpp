#include "PenetrationResolver.h"
#include "Mesh.h"
#include "SparseLinearEquationSolver.h"
#include "KNNSHelper.h"
/*#include <fstream>*/

PenetrationResolver::PenetrationResolver(void)
{
	mRigidMesh = nullptr;
	mKNNSHelper = nullptr;
	mEPS = -0.5;
}

PenetrationResolver::~PenetrationResolver(void)
{
}

void PenetrationResolver::setRigidMesh( Mesh_ rigid )
{
	mRigidMesh = rigid;
	std::vector<Vec3d> pointCloud;	
	pointCloud.reserve(rigid->n_vertices());
	for(Mesh::VertexIter it = rigid->vertices_begin(); it != rigid->vertices_end(); it++){
		pointCloud.push_back(rigid->point(*it));
	}
	mKNNSHelper = std::make_shared<KNNSHelper>(pointCloud);
}

/* Ax = b, x is transpose of (x0 ... xn y0 ... yn z0 ... zn) */
bool PenetrationResolver::resolve(std::vector<Vec3d>& points)
{
	if(mRigidMesh == nullptr){
		PRINT_ERROR("In PenetrationResolver::resolve, mRigidMesh is null");
		return false;
	}
	int pointCount = points.size();
	std::vector<double> rightB;
	std::vector< std::vector< std::pair<int, double> > > leftMatrix;//稀疏矩阵，pair为下标和值
	std::vector<bool> penetrationTest;// = new bool[garVerCount];
	//memset(penetrationTest, 0, garVerCount);
	std::vector<int> nearestHumanVertex;
	computeNNSHumanVertex(nearestHumanVertex, points);
	computePenetrationVertices(nearestHumanVertex, penetrationTest, points);
	/** 构造左矩阵和右向量 **/
	for(int i = 0; i < pointCount; i++){
		auto curVertex = points[i].values_;
		/** 一行 **/
		std::vector< std::pair<int, double> > row;	
		if(penetrationTest[i] == false){						
			for (int j = 0; j < 3; j++){
				row.clear();
				row.push_back(std::make_pair(i+j*pointCount,1));
				leftMatrix.push_back(row);
				rightB.push_back(curVertex[j]);
			}
		}
		else{
			auto curNNSHumanVertexIndex = nearestHumanVertex.at(i);
			auto curNNSHumanVertex = mRigidMesh->point(OpenMesh::VertexHandle(curNNSHumanVertexIndex)).values_;
			auto curNNSHumanNormal = mRigidMesh->normal(OpenMesh::VertexHandle(curNNSHumanVertexIndex)).values_;
			row.clear();
			row.push_back(std::make_pair(i,curNNSHumanNormal[0]));
			row.push_back(std::make_pair(i+pointCount,curNNSHumanNormal[1]));
			row.push_back(std::make_pair(i+2*pointCount,curNNSHumanNormal[2]));
			leftMatrix.push_back(row);
			rightB.push_back(curNNSHumanNormal[0]*curNNSHumanVertex[0]
			+curNNSHumanNormal[1]*curNNSHumanVertex[1]
			+curNNSHumanNormal[2]*curNNSHumanVertex[2]
			-mEPS);
		}
	}
	/** 加入约束，一个顶点的形变与它周围顶点的平均形变相似**/
	if(mTargetAdjList.size() > 0){
		for (int i = 0; i < pointCount; i++){
			if(!penetrationTest[i])
				continue;
			std::vector< std::pair<int, double> > row;
			std::vector<size_t>& adjs = mTargetAdjList[i];
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
				row.push_back(std::make_pair(i+k*pointCount,1));			
				for (int j = 0; j < penetratedAdjSize; j++){
					row.push_back(std::make_pair(penetratedAdj.at(j)+k*pointCount, -1.0/penetratedAdjSize));
				}
				leftMatrix.push_back(row);
			}
			auto curVertex = points[i].values_;
			for (int k = 0; k < 3; k++){
				double adjSum = 0;
				for (int j = 0; j < penetratedAdjSize; j++){
					adjSum += points[penetratedAdj.at(j)].values_[k];
				}
				rightB.push_back(curVertex[k]-1.0/penetratedAdjSize*adjSum);
			}
		}
	}
	std::vector<double> ret;
	ret.reserve(3*pointCount);
	bool isSuccess = SparseLinearEquationSolver().solve(leftMatrix,rightB, 3*pointCount, ret);
	if(isSuccess){		
		int index = 0;
		for(auto v_it = points.begin(); v_it != points.end(); v_it++){
			Mesh::Point& p = *v_it;
			p.values_[0] = ret[index];
			p.values_[1] = ret[index+pointCount];
			p.values_[2] = ret[index+pointCount*2];
			index += 1;
		}
		return true;
	}
	else
		return false;
}

void PenetrationResolver::computeNNSHumanVertex( std::vector<int>& nearestHumanVertex, std::vector<Vec3d>& points  )
{
	if(mRigidMesh == nullptr){
		PRINT_ERROR("In PenetrationResolver::computeNNSHumanVertex, mRigidMesh is null");
		return;
	}
	size_t pointCount = points.size();
	nearestHumanVertex.clear();
	nearestHumanVertex.resize(pointCount, -1);
	KNNSHelper::Result result;
	for(size_t i = 0; i < pointCount; i++){
		Vec3d& query = points[i];
		mKNNSHelper->singleNeighborSearch(query, result);
		nearestHumanVertex[i] = result.index;
	}

// 	std::ofstream out("../data/nnmap.cg");
// 	out << "# D:3 NV:" << pointCount*2
// 		<< " NE:" << pointCount << "\n";
// 	for(size_t i = 0; i < pointCount; i++){
// 		auto& p = mGarment->point(Mesh::VertexHandle(i)).values_;
// 		out << "v " << p[0] << " " << p[1] << " "
// 			<< p[2] << "\n";
// 		size_t n = nearestHumanVertex[i];
// 		auto& hp = mHuman->point(Mesh::VertexHandle(n)).values_;
// 		out << "v " << hp[0] << " " << hp[1] << " "
// 			<< hp[2] << "\n";
// 	}
// 	for(size_t i = 0; i < pointCount; i++){
// 		out << "e " << 2*i+1 << " " << 2*i+2 << "\n";
// 	}
// 	out.close();
}

void PenetrationResolver::computePenetrationVertices( const std::vector<int>& nearestHumanVertex, 
													 std::vector<bool>& penetrationTest, std::vector<Vec3d>& points  )
{
	size_t pointCount = points.size();
	penetrationTest.resize(pointCount, false);
	if(mRigidMesh == nullptr){
		PRINT_ERROR("In PenetrationResolver::computePenetrationVertices, mRigidMesh is null");
		return;
	}
	for(size_t i = 0; i < pointCount; i++){
		Vec3d& garVer = points[i];
		int humIndex = nearestHumanVertex[i];
		if(humIndex == -1){
			PRINT_ERROR("In PenetrationResolver::computePenetrationVertices, humIndex == -1");
			continue;
		}
		const Vec3d& humanVer = mRigidMesh->point(Mesh::VertexHandle(humIndex));
		const Vec3d& humNormal = mRigidMesh->normal(Mesh::VertexHandle(humIndex));
		double dotProduct = (garVer-humanVer)|humNormal;
		if(dotProduct < 0)
			penetrationTest[i] = true;
	}

// 	std::vector<size_t> isPene;
// 	for(size_t i = 0; i < mGarment->n_vertices(); i++){
// 		if(penetrationTest[i]){
// 			isPene.push_back(i);
// 		}
// 	}
// 	std::ofstream out("../data/penetration.cg");
// 	out << "# D:3 NV:" << isPene.size()
// 		<< " NE:" << 0 << "\n";
// 	for(size_t i = 0; i < isPene.size(); i++){
// 		auto& p = mGarment->point(Mesh::VertexHandle(isPene[i])).values_;
// 		out << "v " << p[0] << " " << p[1] << " "
// 			<< p[2] << "\n";
// 	}
// 	out.close();
}

void PenetrationResolver::setAllowDis( double eps )
{
	mEPS = -eps;
}

void PenetrationResolver::setAdjList( std::vector<std::vector<size_t> >& adjList )
{
	mTargetAdjList = adjList;
}
