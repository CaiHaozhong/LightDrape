#include "GeodesicAdapter.h"
#include "Mesh.h"
#include <climits>
GeodesicAdapter::GeodesicAdapter()
{
	_algorithm = nullptr;
}

GeodesicAdapter::GeodesicAdapter( std::vector<double>& pointList, std::vector<unsigned>& faceList )
{
	_mesh.initialize_mesh_data(pointList,faceList);
	_algorithm = new geodesic::GeodesicAlgorithmExact(&_mesh);
}

GeodesicAdapter::GeodesicAdapter( Mesh& _meshOfOpenMesh )
{
	std::vector<double> pointList;
	std::vector<unsigned> faceList;
	/*		int source;*/
// 	double maxYValue = std::numeric_limits<double>::min(), 
// 		minYValue = std::numeric_limits<double>::max();
	for (OpenMesh::PolyConnectivity::VertexIter v_it = _meshOfOpenMesh.vertices_begin();
		v_it != _meshOfOpenMesh.vertices_end(); ++v_it)
	{
		auto point = _meshOfOpenMesh.point(*v_it);				
		for (int i = 0; i < 3; i++)
		{
			pointList.push_back(point.values_[i]);
		}
// 		if(point.values_[1] > maxYValue)
// 		{
// 			maxYValue = point.values_[1];
// 		}		
// 		if(point.values_[1] < minYValue)
// 		{
// 			minYValue = point.values_[1];
// 		}
	}		
	for(OpenMesh::PolyConnectivity::FaceIter f_it = _meshOfOpenMesh.faces_begin();
		f_it != _meshOfOpenMesh.faces_end(); ++f_it)
	{				
		for(OpenMesh::PolyConnectivity::FaceVertexIter fv_it = _meshOfOpenMesh.fv_iter(*f_it);
			fv_it.is_valid(); ++fv_it)
		{
			OpenMesh::ArrayKernel::VertexHandle vertexHandle = *fv_it;
			int index = vertexHandle.idx();

			faceList.push_back(index);				
		}
	}

	_mesh.initialize_mesh_data(pointList,faceList);
	_algorithm = new geodesic::GeodesicAlgorithmExact(&_mesh);
}

GeodesicAdapter::~GeodesicAdapter()
{
	if(_algorithm != nullptr)
		delete _algorithm;
}

void GeodesicAdapter::addSource( unsigned int s )
{
	_all_sources.push_back(&_mesh.vertices()[s]);
}

void GeodesicAdapter::computeGeodesicFromSource( std::vector<double>& ret )
{
	if(!_algorithm)
		return;

	if(_all_sources.size() == 0)
		return;

	_algorithm->propagate(_all_sources);

	ret.clear();
	for(unsigned i = 0; i < _mesh.vertices().size(); ++i)
	{
		geodesic::SurfacePoint p(&_mesh.vertices()[i]);		

		double distance;
		//for a given surface point, find closets source and distance to this source
		unsigned best_source = _algorithm->best_source(p,distance);

		ret.push_back(distance);
	}
}

void GeodesicAdapter::setSource( unsigned int s )
{
	_all_sources.clear();
	addSource(s);
}

double GeodesicAdapter::averageGeodesicDisFromSource( unsigned int s )
{
	if(!_algorithm)
		return 0;

	setSource(s);

	_algorithm->propagate(_all_sources);

	double sum = 0;
	size_t vCount = _mesh.vertices().size();
	for(unsigned i = 0; i < vCount; ++i)
	{
		geodesic::SurfacePoint p(&_mesh.vertices()[i]);		

		double distance;
		
		_algorithm->best_source(p,distance);

		sum += distance;
	}

	return sum / (vCount - 1);
}
