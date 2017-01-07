//Copyright (C) 2008 Danil Kirsanov, MIT License
#ifndef GEODESIC_ALGORITHM_DIJKSTRA_010506
#define GEODESIC_ALGORITHM_DIJKSTRA_010506

#include "geodesic_algorithm_graph_base.hh"
#include "geodesic_mesh_elements.hh"
#include <vector>
#include <set>
#include <assert.h>

namespace geodesic{

class DijkstraNode
{
	typedef DijkstraNode* node_pointer;
public: 
	DijkstraNode(){};
	~DijkstraNode(){};

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

	double distance(SurfacePoint* p)
	{
		return m_vertex->distance(p);
	}

	SurfacePoint surface_point()
	{
		return SurfacePoint(m_vertex);
	}

private: 
	double m_distance;					//distance to the closest source
	unsigned m_source_index;			//closest source index
	node_pointer m_previous;			//previous node in the geodesic path
	vertex_pointer m_vertex;			//correspoding vertex
};

class GeodesicAlgorithmDijkstra: public GeodesicAlgorithmGraphBase<DijkstraNode>
{
public:
	typedef DijkstraNode Node;
	typedef Node* node_pointer;

	GeodesicAlgorithmDijkstra(geodesic::Mesh* mesh):
		GeodesicAlgorithmGraphBase<Node>(mesh)
	{
		m_type = DIJKSTRA;

		m_nodes.resize(mesh->vertices().size());
		for(unsigned i=0; i<m_nodes.size(); ++i)
		{
			m_nodes[i].vertex() = &m_mesh->vertices()[i];
		}
	};

	~GeodesicAlgorithmDijkstra(){};

protected:

	void list_nodes_visible_from_source(MeshElementBase* p, 
										std::vector<node_pointer>& storage);		//list all nodes that belong to this mesh element

	void list_nodes_visible_from_node(node_pointer node,			//list all nodes that belong to this mesh element
									  std::vector<node_pointer>& storage,
									  std::vector<double>& distances, 
									  double threshold_distance);	//list only the nodes whose current distance is larger than the threshold
};

}		//geodesic

#endif //GEODESIC_ALGORITHM_DIJKSTRA_010506
