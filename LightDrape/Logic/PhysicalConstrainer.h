#pragma once
#include "MeshContainer.h"
class PhysicalConstrainer
{
public:
	PhysicalConstrainer();
	~PhysicalConstrainer();
	void physicalSimulate(double dt);
	
	/** Simulate **/
	void gravitiySimulate(float dt);
	void edgeSatisfyConstraints();
private:
	double mCurTime;
	Mesh* mClothMesh;
	std::vector<Mesh::Point> mOldPosList;
	std::vector<double> mEdgeRestLenList;
	OpenMesh::Vec3f mAcceleration;

private:
	/** Copy vertices' postion to mOldPosList **/
	void storeVertices(std::vector<Mesh::Point>& dest);

	/** Store all edges' initial length to mEdgeRestLenList **/
	void initEdgeRestLenList();

	/** ����3s����λ��Ϊ�·��ĸ߶ȣ�������ٶ� (S = v0*t + 1/2*a*t*t) **/
	void initialAcceleration();

};

