#include "geodesic_algorithm_subdivision.hh"

namespace geodesic{
	void GeodesicAlgorithmSubdivision::list_nodes(MeshElementBase* p,
		std::vector<node_pointer>& storage,
		double threshold_distance)
	{
		assert(p->type() != UNDEFINED_POINT);

		if(p->type() == VERTEX)
		{
			vertex_pointer v = static_cast<vertex_pointer>(p);
			node_pointer node = &m_nodes[node_index(v)];
			if(node->distance_from_source() > threshold_distance)
			{
				storage.push_back(node);
			}
		}
		else if(p->type() == EDGE)
		{
			edge_pointer e = static_cast<edge_pointer>(p);
			unsigned node_index = node_indexx(e);
			for(unsigned i=0; i<m_subdivision_level; ++i)
			{
				node_pointer node = &m_nodes[node_index++];
				if(node->distance_from_source() > threshold_distance)
				{
					storage.push_back(node);
				}
			}
		}
		//FACE has no nodes
	}

	void GeodesicAlgorithmSubdivision::list_nodes_visible_from_source(MeshElementBase* p,
		std::vector<node_pointer>& storage)
	{
		assert(p->type() != UNDEFINED_POINT);

		if(p->type() == FACE)
		{
			face_pointer f = static_cast<face_pointer>(p);
			for(unsigned i=0; i<3; ++i)
			{
				list_nodes(f->adjacent_vertices()[i],storage);
				list_nodes(f->adjacent_edges()[i],storage);
			}
		}
		else if(p->type() == EDGE)
		{
			list_nodes(p,storage);
			list_nodes(p->adjacent_vertices()[0],storage);
			list_nodes(p->adjacent_vertices()[1],storage);
		}
		else			//VERTEX
		{
			list_nodes(p,storage);
		}
	}

	void GeodesicAlgorithmSubdivision::list_nodes_visible_from_node(node_pointer node, //list all nodes that belong to this mesh element
		std::vector<node_pointer>& storage,
		std::vector<double>& distances,
		double threshold_distance)
	{
		MeshElementBase* p = node->base_element();
		assert(p->type() != UNDEFINED_POINT);
		assert(storage.size() == distances.size());

		if(p->type() == VERTEX)
		{
			vertex_pointer v = static_cast<vertex_pointer>(p);

			for(unsigned i=0; i<v->adjacent_edges().size(); ++i)
			{
				edge_pointer e = v->adjacent_edges()[i];
				vertex_pointer v_opposite = e->opposite_vertex(v);
				list_nodes(e, storage, threshold_distance);
				list_nodes(v_opposite, storage, threshold_distance);
			}
			for(unsigned i=0; i<v->adjacent_faces().size(); ++i)
			{
				face_pointer f = v->adjacent_faces()[i];
				edge_pointer e = f->opposite_edge(v);
				list_nodes(e, storage, threshold_distance);
			}
		}
		else if(p->type() == EDGE)
		{
			edge_pointer e = static_cast<edge_pointer>(p);

			vertex_pointer v0 = e->adjacent_vertices()[0];
			vertex_pointer v1 = e->adjacent_vertices()[1];
			list_nodes(v0, storage, threshold_distance);
			list_nodes(v1, storage, threshold_distance);

			for(unsigned i=0; i<e->adjacent_faces().size(); ++i)
			{
				face_pointer f = e->adjacent_faces()[i];

				list_nodes(f->next_edge(e,v0), storage, threshold_distance);
				list_nodes(f->next_edge(e,v1), storage, threshold_distance);
				list_nodes(f->opposite_vertex(e), storage, threshold_distance);
			}
		}
		else 
		{
			assert(0);
		}

		unsigned index = distances.size();
		distances.resize(storage.size());
		for(; index<storage.size(); ++index)
		{	
			distances[index] = node->distance(&storage[index]->surface_point());
		}
	}

}