#pragma once
#include <vector>
#include <tuple>
#include "geodesic_algorithm_exact.hh"
#include "Mesh.h"
class GeodesicAdapter{
public:
	GeodesicAdapter(){
		_algorithm = nullptr;
	}
	GeodesicAdapter(std::vector<double>& pointList, std::vector<unsigned>& faceList){
		_mesh.initialize_mesh_data(pointList,faceList);
		_algorithm = new geodesic::GeodesicAlgorithmExact(&_mesh);
	}

	
	//************************************
	// FullName:        GeodesicAdapter<MeshOFOPENMESH>::GeodesicAdapter
	// Access:          public 
	// Returns:         
	// Parameter:       MeshOFOPENMESH & _meshOfOpenMesh
	// Description:     Don't have to invoke addSource(...) out of this constrctor
	//************************************
	GeodesicAdapter(Mesh& _meshOfOpenMesh){
		std::vector<double> pointList;
		std::vector<unsigned> faceList;
/*		int source;*/
		double maxYValue = -1000000, minYValue = 1000000;				
		for (OpenMesh::PolyConnectivity::VertexIter v_it = _meshOfOpenMesh.vertices_begin(); v_it != _meshOfOpenMesh.vertices_end(); ++v_it)
		{
			auto point = _meshOfOpenMesh.point(*v_it);				
			for (int i = 0; i < 3; i++)
			{
				pointList.push_back(point.values_[i]);
			}
			if(point.values_[1] > maxYValue)
			{
				maxYValue = point.values_[1];
			}		
			if(point.values_[1] < minYValue)
			{
				minYValue = point.values_[1];
			}
		}		
		for(OpenMesh::PolyConnectivity::FaceIter f_it = _meshOfOpenMesh.faces_begin(); f_it != _meshOfOpenMesh.faces_end(); ++f_it)
		{				
			for(OpenMesh::PolyConnectivity::FaceVertexIter fv_it = _meshOfOpenMesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
			{
				OpenMesh::ArrayKernel::VertexHandle vertexHandle = *fv_it;
				int index = vertexHandle.idx();

				faceList.push_back(index);				
			}
		}

		_mesh.initialize_mesh_data(pointList,faceList);
		_algorithm = new geodesic::GeodesicAlgorithmExact(&_mesh);

// 		double threshold = maxYValue - (maxYValue-minYValue)/50;
// 		typedef std::pair<MeshOFOpenMesh::Point, size_t> Node;
// 		std::vector<Node> candidatePoints;
// 		int cur = 0;
// 		double minX = 1000000, maxX = -minX, maxZ = minX, minZ = -maxZ;
// 		for (OpenMesh::PolyConnectivity::VertexIter v_it = _meshOfOpenMesh.vertices_begin(); v_it != _meshOfOpenMesh.vertices_end(); ++v_it)
// 		{
// 			MeshOFOpenMesh::Point& p = _meshOfOpenMesh.point(*v_it);
// 			if(p.values_[1] >= threshold){
// 				candidatePoints.push_back(std::make_pair(p, cur));
// 				if(p.values_[0] > maxX) maxX = p.values_[0];
// 				if(p.values_[0] < minX) minX = p.values_[0];
// 				if(p.values_[2] > maxZ) maxZ = p.values_[2];
// 				if(p.values_[2] < minZ) minZ = p.values_[2];
// 			}
// 			cur++;
// 		}
// 		MeshOFOpenMesh::Point centerTopPoint = MeshOFOpenMesh::Point((maxX+minX)/2, maxYValue, (maxZ+minZ)/2);
// 		double dis = 1000000;
// 		Node sourceNode;
// 		for(size_t i = 0; i < candidatePoints.size(); i++)
// 		{
// 			MeshOFOpenMesh::Point& curPoint = candidatePoints[i].first;
// 			double dx = (curPoint.values_[0]-centerTopPoint.values_[0]);
// 			double dy = (curPoint.values_[1]-centerTopPoint.values_[1]);
// 			double dz = (curPoint.values_[2]-centerTopPoint.values_[2]);
// 			double d = dx*dx + dy*dy + dz*dz;
// 			if(d < dis)
// 			{
// 				dis = d;
// 				sourceNode = candidatePoints[i];
// 			}
// 		}
// 		addSource(sourceNode.second);
	}
	
	~GeodesicAdapter()
	{
		if(_algorithm != nullptr)
			delete _algorithm;
	}

	/* 添加一个测地源点 */
	void addSource(unsigned int s)
	{
		_all_sources.push_back(&_mesh.vertices()[s]);
	}

	void computeGeodesicFromSource(std::vector<double>& ret)
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
			unsigned best_source = _algorithm->best_source(p,distance);		//for a given surface point, find closets source and distance to this source

			ret.push_back(distance);
		}
	}
private:
	geodesic::Mesh _mesh;
	geodesic::GeodesicAlgorithmExact* _algorithm;
	std::vector<geodesic::SurfacePoint> _all_sources;
};

