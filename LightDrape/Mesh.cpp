#include "Mesh.h"
#include "VertexAlter.h"
#include "MeshMaterial.h"

Mesh::Mesh() :_Mesh(){
	mHasRequestAABB = false;
	float maxFloat = std::numeric_limits<float>::max();
	float minFloat = std::numeric_limits<float>::min();
	mMinPoint = OpenMesh::Vec3f(maxFloat,maxFloat,maxFloat);
	mMaxPoint = OpenMesh::Vec3f(minFloat,minFloat,minFloat);
	mVertexAlter = smartNew(VertexAlter);
	mEdgeAverageLength = -1;
}

Mesh::~Mesh() {
}

void Mesh::requestRecomputeAABB(){
	mHasRequestAABB = false;
	requestAABB();
}

void Mesh::requestAABB(){
	if(mHasRequestAABB)
		return;
	for(Mesh::ConstVertexIter vIt = this->vertices_begin(); vIt != this->vertices_end(); vIt++){
		const Mesh::Point& p = this->point(*vIt);
		mMinPoint.minimize(OpenMesh::vector_cast<OpenMesh::Vec3d>(p));
		mMaxPoint.maximize(OpenMesh::vector_cast<OpenMesh::Vec3d>(p));
	}
	mHasRequestAABB = true;
}

Vec3d Mesh::getMin(){
	requestAABB();
	return mMinPoint;
}

Vec3d Mesh::getMax(){
	requestAABB();
	return mMaxPoint;
}

double Mesh::getDigonalLen(){
	requestAABB();
	return (mMaxPoint-mMinPoint).length();
}

double Mesh::getWidth(){
	requestAABB();
	return mMaxPoint.values_[0] - mMinPoint.values_[0];
}

double Mesh::getHeight(){
	requestAABB();
	return mMaxPoint.values_[1] - mMinPoint.values_[1];
}

double Mesh::getLength(){
	requestAABB();
	return mMaxPoint.values_[2] - mMinPoint.values_[2];
}

void Mesh::registerProperty( Registerable_ vp )
{
	vp->registerSelf(mPropertyManager);
}

void Mesh::initProperty()
{
	mPropertyManager = smartNew(PropertyManager);
	mPropertyManager->init(n_vertices());
}

std::pair<size_t, size_t> Mesh::getEndVertices( EdgeHandle e )
{
	Mesh::HalfedgeHandle halfEdge = this->halfedge_handle(e, 0);
	Mesh::VertexHandle fromV = this->from_vertex_handle(halfEdge);
	Mesh::VertexHandle toV = this->to_vertex_handle(halfEdge);
	return std::make_pair(fromV.idx(), toV.idx());
}

double Mesh::getEdgeLength( EdgeHandle e )
{
	std::pair<size_t, size_t> vs = getEndVertices(e);
	Vec3d a = this->point(VertexHandle(vs.first));
	Vec3d b = this->point(VertexHandle(vs.second));
	return (a-b).length();
}

std::string Mesh::getName() const
{
	return mName;
}

void Mesh::setName( std::string val )
{
	mName = val;
}

void Mesh::alterVertex(size_t index, const Vec3d& delta)
{
	mVertexAlter->add(index, delta);
}

double Mesh::getAverageEdgeLength()
{
	if(mEdgeAverageLength > 0){
		return mEdgeAverageLength;
	}
	mEdgeAverageLength = 0;
	for(Mesh::EdgeIter e_it = this->edges_begin(); e_it != this->edges_end();
		e_it++){
			mEdgeAverageLength += getEdgeLength(*e_it);
	}
	mEdgeAverageLength = mEdgeAverageLength / this->n_edges();
	return mEdgeAverageLength;
}

MeshMaterial_ Mesh::getMeshMaterial() const { return mMeshMaterial; }

void Mesh::setMeshMaterial(MeshMaterial_ val) { mMeshMaterial = val; }