#include "GarmentPenetrationResolver.h"
#include "Mesh.h"
#include "PenetrationResolver.h"

GarmentPenetrationResolver::GarmentPenetrationResolver(void)
{
	this->mGarment = nullptr;
	this->mHuman = nullptr;
	mPenetrationResolver = smartNew(PenetrationResolver);
}

GarmentPenetrationResolver::GarmentPenetrationResolver( Mesh_ garment )
{	
	this->mHuman = nullptr;
	mPenetrationResolver = smartNew(PenetrationResolver);
	setGarment(garment);
}


GarmentPenetrationResolver::~GarmentPenetrationResolver(void)
{

}

bool GarmentPenetrationResolver::resolve()
{
	if(mGarment == nullptr){
		PRINT_ERROR("In GarmentPenetrationResolver::resolve, mGarment is null");
		return false;
	}
	if(mHuman == nullptr){
		PRINT_ERROR("In GarmentPenetrationResolver::resolve, mHuman is null");
		return false;
	}
	std::vector<Vec3d> points;
	for(auto it = mGarment->vertices_begin(); it != mGarment->vertices_end(); it++){
		points.push_back(mGarment->point(*it));
	}
	mPenetrationResolver->resolve(points);
	size_t cur = 0;
	for(auto it = mGarment->vertices_begin(); it != mGarment->vertices_end(); it++){
		Vec3d& p = mGarment->point(*it);
		Vec3d& np = points[cur++];
		p[0] = np[0];
		p[1] = np[1];
		p[2] = np[2];
	}
	return true;
}

void GarmentPenetrationResolver::setHuman( Mesh_ human )
{
	mHuman = human;
	mPenetrationResolver->setRigidMesh(mHuman);
};

void GarmentPenetrationResolver::setGarment( Mesh_ garment )
{
	mGarment = garment;
	std::vector< std::vector<size_t> > adjList;
	computeAdjList(garment, adjList);
	mPenetrationResolver->setAdjList(adjList);
}

void GarmentPenetrationResolver::computeAdjList( Mesh_ mesh, std::vector< std::vector<size_t> >& adjList )
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