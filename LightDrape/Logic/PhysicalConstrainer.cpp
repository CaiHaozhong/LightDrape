#include "PhysicalConstrainer.h"
#include <ctime>
PhysicalConstrainer::PhysicalConstrainer():mClothMesh(&(globalMeshContatiner.getMeshRef(1)))
{
	mCurTime = 0;
	initialAcceleration();
	storeVertices(mOldPosList);
	initEdgeRestLenList();
}

PhysicalConstrainer::~PhysicalConstrainer()
{
}

void PhysicalConstrainer::physicalSimulate( double dt )
{
	time_t start, end;
	start = clock();
	gravitiySimulate(dt);
	end = clock();
	printf("Tick Time: %ld\n", (end-start));
	edgeSatisfyConstraints();
}

void PhysicalConstrainer::gravitiySimulate( float dt )
{
	printf("dt: %f\n",dt);
	float step = dt / 1000;
	mCurTime += step;
	printf("mCurTime: %f\n",mCurTime);
	std::vector<Mesh::Point> curPosList;
	storeVertices(curPosList);
	int i = 0;
	Mesh::VertexIter vIt = mClothMesh->vertices_begin();
	Mesh::Point& point = mClothMesh->point(*vIt);
	Mesh::Point oldPoint = mOldPosList.at(i);
	printf("curPoint:(%f,%f,%f)\n", point.values_[0],point.values_[1],point.values_[2]);
	printf("oldPoint:(%f,%f,%f)\n", oldPoint.values_[0],oldPoint.values_[1],oldPoint.values_[2]);
	for(; vIt != mClothMesh->vertices_end(); vIt++, i++)
	{
		Mesh::Point& point = mClothMesh->point(*vIt);
		Mesh::Point oldPoint = mOldPosList.at(i);
		point += (point - oldPoint + mAcceleration * step * step);
	}
	std::copy(curPosList.begin(), curPosList.end(), mOldPosList.begin());
}

void PhysicalConstrainer::edgeSatisfyConstraints()
{
	int i = 0;
	for(Mesh::EdgeIter eIt = mClothMesh->edges_begin(); eIt != mClothMesh->edges_end(); eIt++, i++)
	{
		Mesh::HalfedgeHandle h0 = mClothMesh->halfedge_handle(*eIt, 0);
		Mesh::HalfedgeHandle h1 = mClothMesh->halfedge_handle(*eIt, 1);
		Mesh::VertexHandle v0 = mClothMesh->to_vertex_handle(h0);
		Mesh::VertexHandle v1 = mClothMesh->to_vertex_handle(h1);
		Mesh::Point& p0 = mClothMesh->point(v0);
		Mesh::Point& p1 = mClothMesh->point(v1);
		float len = (p1-p0).length();
		double restLen = mEdgeRestLenList.at(i);
		Mesh::Point direction = (p1-p0)/len;
		double halfDelta = (len - restLen) / 2;
		p0 += direction * halfDelta;
 		p1 -= direction * halfDelta;
	}
}

void PhysicalConstrainer::initEdgeRestLenList()
{
	for(Mesh::EdgeIter eIt = mClothMesh->edges_begin(); eIt != mClothMesh->edges_end(); eIt++)
	{
		Mesh::HalfedgeHandle h0 = mClothMesh->halfedge_handle(*eIt, 0);
		Mesh::HalfedgeHandle h1 = mClothMesh->halfedge_handle(*eIt, 1);
		Mesh::VertexHandle v0 = mClothMesh->to_vertex_handle(h0);
		Mesh::VertexHandle v1 = mClothMesh->to_vertex_handle(h1);
		Mesh::Point p0 = mClothMesh->point(v0);
		Mesh::Point p1 = mClothMesh->point(v1);
		mEdgeRestLenList.push_back((p1-p0).length());
	}
}
/** 根据3s下落位移为衣服的高度，计算加速度 (S = v0*t + 1/2*a*t*t) **/
void PhysicalConstrainer::initialAcceleration()
{
	mClothMesh->requestAABB();
	float meshHeight = mClothMesh->getHeight();
	mAcceleration = OpenMesh::Vec3f(0, -2 * meshHeight / 9.0f, 0);
	printf("meshHeight: %f\n", meshHeight);
	printf("Acceleration: (0, %f, 0)\n", mAcceleration.values_[1]);
}

void PhysicalConstrainer::storeVertices( std::vector<Mesh::Point>& dest )
{
	dest.clear();
	for(Mesh::VertexIter vIt = mClothMesh->vertices_begin(); vIt != mClothMesh->vertices_end(); vIt++)
	{
		dest.push_back(mClothMesh->point(*vIt));
	}
}
