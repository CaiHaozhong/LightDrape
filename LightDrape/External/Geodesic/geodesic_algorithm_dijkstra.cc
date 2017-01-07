#include "geodesic_algorithm_dijkstra.hh"
void geodesic::GeodesicAlgorithmDijkstra::list_nodes_visible_from_source(MeshElementBase* p,
															   std::vector<node_pointer>& storage)
{
	assert(p->type() != UNDEFINED_POINT);

	if(p->type() == FACE)
	{
		face_pointer f = static_cast<face_pointer>(p);
		for(unsigned i=0; i<3; ++i)
		{
			vertex_pointer v = f->adjacent_vertices()[i];
			storage.push_back(&m_nodes[node_index(v)]);
		}
	}
	else if(p->type() == EDGE)
	{
		edge_pointer e = static_cast<edge_pointer>(p);
		for(unsigned i=0; i<2; ++i)
		{
			vertex_pointer v = e->adjacent_vertices()[i];
			storage.push_back(&m_nodes[node_index(v)]);
		}
	}
	else			//VERTEX
	{
		vertex_pointer v = static_cast<vertex_pointer>(p);
		storage.push_back(&m_nodes[node_index(v)]);
	}
}

void geodesic::GeodesicAlgorithmDijkstra::list_nodes_visible_from_node(node_pointer node, //list all nodes that belong to this mesh element
																	std::vector<node_pointer>& storage,
																	std::vector<double>& distances,
																	double threshold_distance)
{
	vertex_pointer v = node->vertex();
	assert(storage.size() == distances.size());

	for(unsigned i=0; i<v->adjacent_edges().size(); ++i)
	{
		edge_pointer e = v->adjacent_edges()[i];
		vertex_pointer new_v = e->opposite_vertex(v);
		node_pointer new_node = &m_nodes[node_index(new_v)];

		if(new_node->distance_from_source() > threshold_distance + e->length())
		{
			storage.push_back(new_node);
			distances.push_back(e->length());
		}
	}
}