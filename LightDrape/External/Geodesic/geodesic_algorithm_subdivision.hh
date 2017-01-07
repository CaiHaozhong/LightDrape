//Copyright (C) 2008 Danil Kirsanov, MIT License
#ifndef GEODESIC_ALGORITHM_SUBDIVISION_122806
#define GEODESIC_ALGORITHM_SUBDIVISION_122806

#include "geodesic_algorithm_graph_base.hh"
#include "geodesic_mesh_elements.hh"
#include <vector>
#include <set>
#include <assert.h>

namespace geodesic{

class SubdivisionNode: public SurfacePoint
{
	typedef SubdivisionNode* node_pointer;
public: 
	SubdivisionNode(){};

	template <class Pointer>
	SubdivisionNode(Pointer p):
		SurfacePoint(p),
		m_previous(NULL),
		m_distance(0.0)
	{};

	template <class Pointer, class Parameter>
	SubdivisionNode(Pointer p, Parameter param):
		SurfacePoint(p, param),
		m_previous(NULL),
		m_distance(0.0)
	{};

	~SubdivisionNode(){};

	double& distance_from_source(){return m_distance;};
	node_pointer& previous(){return m_previous;};
	unsigned& source_index(){return m_source_index;};

	void clear()
	{
		m_distance = GEODESIC_INF;
		m_previous = NULL;
	}

	bool operator()(node_pointer const s1, node_pointer const s2) const
	{
		if(s1 == s2)
		{
			return false;
		}
		if(s1->distance_from_source() != s2->distance_from_source())
		{
			return s1->distance_from_source() < s2->distance_from_source();
		}
/*		if(s1->type() != s2->type())
		{
			return s1->type() < s2->type();
		}
		if(s1->base_element()->id() != s2->base_element()->id())
		{
		    return s1->base_element()->id() < s2->base_element()->id();
		} */
		if(s1->x() != s2->x())		//two nodes cannot be located in the same space 
		{
			return s1->x() < s2->x();
		}
		if(s1->y() != s2->y())		
		{
			return s1->y() < s2->y();
		}
		if(s1->z() != s2->z())		
		{
			return s1->z() < s2->z();
		}

		assert(0);
		return true;
	};

	SurfacePoint& surface_point(){return static_cast<SurfacePoint&>(*this);};

private: 
	double m_distance;					//distance to the closest source
	unsigned m_source_index;			//closest source index
	node_pointer m_previous;			//previous node in the geodesic path
};

class GeodesicAlgorithmSubdivision: public GeodesicAlgorithmGraphBase<SubdivisionNode>
{
	typedef SubdivisionNode Node;
public:
	GeodesicAlgorithmSubdivision(geodesic::Mesh* mesh = NULL, 
								 unsigned subdivision_level = 0):
		GeodesicAlgorithmGraphBase<Node>(mesh)
	{
		m_type = SUBDIVISION;

		m_nodes.reserve(mesh->vertices().size());
		for(unsigned i=0; i<mesh->vertices().size(); ++i)
		{
			vertex_pointer v = &mesh->vertices()[i];
			m_nodes.push_back(Node(v));		//!!
		}

		set_subdivision_level(subdivision_level);
	};

	~GeodesicAlgorithmSubdivision(){};

	unsigned subdivision_level(){return m_subdivision_level;};

	void set_subdivision_level(unsigned subdivision_level)
	{
		m_subdivision_level = subdivision_level;

		m_nodes.resize(m_mesh->vertices().size());
		m_nodes.reserve(m_mesh->vertices().size() + 
						m_mesh->edges().size()*subdivision_level);

		for(unsigned i=0; i<m_mesh->edges().size(); ++i)
		{
			edge_pointer e = &m_mesh->edges()[i];
			for(unsigned i=0; i<subdivision_level; ++i)
			{
				double offset = (double)(i+1)/(double)(subdivision_level+1);
				m_nodes.push_back(Node(e, offset));
			}
		}
	};

protected:
	void list_nodes_visible_from_source(MeshElementBase* p, 
										std::vector<node_pointer>& storage);		//list all nodes that belong to this mesh element

	void list_nodes_visible_from_node(node_pointer node,			//list all nodes that belong to this mesh element
									  std::vector<node_pointer>& storage,
									  std::vector<double>& distances, 
									  double threshold_distance);	//list only the nodes whose current distance is larger than the threshold
	
	unsigned node_indexx(edge_pointer e)
	{
		return e->id()*m_subdivision_level + m_mesh->vertices().size();
	};

private:
	void list_nodes(MeshElementBase* p,		//list nodes that belong to this mesh element
					std::vector<node_pointer>& storage,
					double threshold_distance = -1.0);				//list only the nodes whose current distance is larger than the threshold

	unsigned m_subdivision_level;	//when level is equal to 1, this algorithm corresponds to the Dijkstra algorithm
};

}		//geodesic

#endif //GEODESIC_ALGORITHM_SUBDIVISION_122806
