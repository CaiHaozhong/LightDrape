#pragma once
#include "geodesic_algorithm_exact.hh"
#include <vector>
#include <tuple>
class Mesh;
class GeodesicAdapter{
public:
	GeodesicAdapter();
	GeodesicAdapter(std::vector<double>& pointList, std::vector<unsigned>& faceList);

	GeodesicAdapter(Mesh& _meshOfOpenMesh);
	
	~GeodesicAdapter();

	/* 添加一个测地源点 */
	void addSource(unsigned int s);

	void computeGeodesicFromSource(std::vector<double>& ret);
private:
	geodesic::Mesh _mesh;
	geodesic::GeodesicAlgorithmExact* _algorithm;
	std::vector<geodesic::SurfacePoint> _all_sources;
};

