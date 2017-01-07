//Copyright (C) 2008 Danil Kirsanov, MIT License
#ifndef GEODESIC_ALGORITHM_DIJKSTRA_ALTERNATIVE_010506
#define GEODESIC_ALGORITHM_DIJKSTRA_ALTERNATIVE_010506

#include "geodesic_algorithm_base.hh"
#include "geodesic_mesh_elements.hh"
#include <vector>
#include <set>
#include <assert.h>

namespace geodesic{

class DijkstraNode1
{
	typedef DijkstraNode1* node_pointer;
public: 
	DijkstraNode1(){};
	~DijkstraNode1(){};

	double& distance_from_source(){return m_distance;};
	node_pointer& previous(){return m_previous;};
	unsigned& source_index(){return m_source_index;};
	vertex_pointer& vertex(){return m_vertex;};

	void clear()
	{
		m_distance = GEODESIC_INF;
		m_previous = NULL;
	}

	bool operator()(node_pointer const s1, node_pointer const s2) const
	{
		return s1->distance_from_source() != s2->distance_from_source() ?
			   s1->distance_from_source() < s2->distance_from_source() :
			   s1->vertex()->id() < s2->vertex()->id();
	};

private: 
	double m_distance;					//distance to the closest source
	unsigned m_source_index;			//closest source index
	node_pointer m_previous;			//previous node in the geodesic path
	vertex_pointer m_vertex;			//correspoding vertex
};

class GeodesicAlgorithmDijkstraAlternative: public GeodesicAlgorithmBase
{
public:
	typedef DijkstraNode1 Node;
	typedef Node* node_pointer;

	GeodesicAlgorithmDijkstraAlternative(geodesic::Mesh* mesh = NULL):
		GeodesicAlgorithmBase(mesh),
		m_nodes(mesh->vertices().size())
	{
		m_type = DIJKSTRA;
		for(unsigned i=0; i<m_nodes.size(); ++i)
		{
			m_nodes[i].vertex() = &m_mesh->vertices()[i];
		}
	};

	~GeodesicAlgorithmDijkstraAlternative(){};

	virtual void propagate(std::vector<SurfacePoint>& sources,
   						   double max_propagation_distance  = GEODESIC_INF,			//propagation algorithm stops after reaching the certain distance from the source
						   std::vector<SurfacePoint>* stop_points = NULL); //or after ensuring that all the stop_points are covered

	virtual void trace_back(SurfacePoint& destination,		//trace back piecewise-linear path
							std::vector<SurfacePoint>& path);

	virtual unsigned best_source(SurfacePoint& point,			//quickly find what source this point belongs to and what is the distance to this source
									double& best_source_distance); 
private:

	void set_sources(std::vector<SurfacePoint>& sources)
	{
		m_sources = sources;
	}

	bool check_stop_conditions(unsigned& index);
	
	std::vector<Node> m_nodes;	//nodes of the subdivision graph located on the vertices

	std::vector<SurfacePoint> m_sources;		//for simplicity, we keep sources as they are

	typedef std::set<node_pointer, Node> queue_type;
	queue_type m_queue;
};

}		//geodesic

#endif //GEODESIC_ALGORITHM_DIJKSTRA_ALTERNATIVE_010506
