//Copyright (C) 2008 Danil Kirsanov, MIT License

#ifndef GEODESIC_ALGORITHM_BASE_122806
#define GEODESIC_ALGORITHM_BASE_122806

#include "geodesic_mesh.hh"
#include "geodesic_constants_and_simple_functions.hh"
#include <iostream>
#include <ctime>

namespace geodesic{

class GeodesicAlgorithmBase
{
public:
    enum AlgorithmType
    {
        EXACT,
		DIJKSTRA,
        SUBDIVISION,
		UNDEFINED_ALGORITHM
    };

	GeodesicAlgorithmBase(geodesic::Mesh* mesh):
		m_type(UNDEFINED_ALGORITHM),
		m_max_propagation_distance(1e100),
		m_mesh(mesh)
	{};	

	virtual ~GeodesicAlgorithmBase(){};

	virtual void propagate(std::vector<SurfacePoint>& sources,
   						   double max_propagation_distance = GEODESIC_INF,			//propagation algorithm stops after reaching the certain distance from the source
						   std::vector<SurfacePoint>* stop_points = NULL) = 0; //or after ensuring that all the stop_points are covered

	virtual void trace_back(SurfacePoint& destination,		//trace back piecewise-linear path
							std::vector<SurfacePoint>& path) = 0;

	void geodesic(SurfacePoint& source,
						  SurfacePoint& destination,
						  std::vector<SurfacePoint>& path); //lazy people can find geodesic path with one function call

	void geodesic(std::vector<SurfacePoint>& sources,
						  std::vector<SurfacePoint>& destinations,
						  std::vector<std::vector<SurfacePoint> >& paths); //lazy people can find geodesic paths with one function call

	virtual unsigned best_source(SurfacePoint& point,			//after propagation step is done, quickly find what source this point belongs to and what is the distance to this source
								 double& best_source_distance) = 0; 

	virtual void print_statistics()		//print info about timing and memory usage in the propagation step of the algorithm
	{
		std::cout << "propagation step took " << m_time_consumed << " seconds " << std::endl;
	};	

	AlgorithmType type(){return m_type;};

	virtual std::string name();

	geodesic::Mesh* mesh(){return m_mesh;};
protected:

	void set_stop_conditions(std::vector<SurfacePoint>* stop_points, 
						     double stop_distance);
	double stop_distance()
	{
		return m_max_propagation_distance;
	}

	AlgorithmType m_type;					   // type of the algorithm

	typedef std::pair<vertex_pointer, double> stop_vertex_with_distace_type;
	std::vector<stop_vertex_with_distace_type> m_stop_vertices; // algorithm stops propagation after covering certain vertices
	double m_max_propagation_distance;			 // or reaching the certain distance

	geodesic::Mesh* m_mesh;

	double m_time_consumed;		//how much time does the propagation step takes
	double m_propagation_distance_stopped;		//at what distance (if any) the propagation algorithm stopped 
};

double length(std::vector<SurfacePoint>& path);

void print_info_about_path(std::vector<SurfacePoint>& path);

}//geodesic

#endif //GEODESIC_ALGORITHM_BASE_122806
