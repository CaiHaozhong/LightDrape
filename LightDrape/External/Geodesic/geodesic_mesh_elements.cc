#include "geodesic_mesh_elements.hh"

geodesic::edge_pointer geodesic::Face::opposite_edge(geodesic::vertex_pointer v)
{
	for(unsigned i=0; i<3; ++i)
	{
		geodesic::edge_pointer e = adjacent_edges()[i];
		if(!e->belongs(v))
		{
			return e;
		}
	}
	assert(0);
	return NULL;
}

geodesic::vertex_pointer geodesic::Face::opposite_vertex(geodesic::edge_pointer e)
{
	for(unsigned i=0; i<3; ++i)
	{
		geodesic::vertex_pointer v = adjacent_vertices()[i];
		if(!e->belongs(v))
		{
			return v;
		}
	}
	assert(0);
	return NULL;
}

geodesic::edge_pointer geodesic::Face::next_edge(geodesic::edge_pointer e, geodesic::vertex_pointer v)
{
	assert(e->belongs(v));

	for(unsigned i=0; i<3; ++i)
	{
		geodesic::edge_pointer next = adjacent_edges()[i];
		if(e->id() != next->id() && next->belongs(v))
		{
			return next;
		}
	}
	assert(0);
	return NULL;
}

bool geodesic::operator < (const geodesic::HalfEdge &x, const geodesic::HalfEdge &y)
{
	if(x.vertex_0 == y.vertex_0)
	{
		return x.vertex_1 < y.vertex_1;
	}
	else
	{
		return x.vertex_0 < y.vertex_0;
	}
}

bool geodesic::operator != (const geodesic::HalfEdge &x, const geodesic::HalfEdge &y)
{
	return x.vertex_0 != y.vertex_0 || x.vertex_1 != y.vertex_1;
}

bool geodesic::operator == (const geodesic::HalfEdge &x, const geodesic::HalfEdge &y)
{
	return x.vertex_0 == y.vertex_0 && x.vertex_1 == y.vertex_1;
}