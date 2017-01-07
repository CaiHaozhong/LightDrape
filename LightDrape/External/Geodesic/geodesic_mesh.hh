//Copyright (C) 2008 Danil Kirsanov, MIT License
#ifndef GEODESIC_MESH_20071231
#define GEODESIC_MESH_20071231

#include <cstddef>
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <fstream>

#include "geodesic_mesh_elements.hh"
#include "geodesic_memory.hh"
#include "geodesic_constants_and_simple_functions.hh"

namespace geodesic{

struct edge_visible_from_source
{
	unsigned source;
	edge_pointer edge;
};

class Mesh
{
public:
	Mesh()
	{};

	~Mesh(){};

	template<class Points, class Faces>
	void initialize_mesh_data(unsigned num_vertices,
							  Points& p, 
							  unsigned num_faces,
							  Faces& tri)		//build mesh from regular point-triangle representation
	{
		{
			unsigned const approximate_number_of_internal_pointers = (num_vertices + num_faces)*4;
			unsigned const max_number_of_pointer_blocks = 100; 
			m_pointer_allocator.reset(approximate_number_of_internal_pointers, 
				max_number_of_pointer_blocks);

			m_vertices.resize(num_vertices);
			for(unsigned i=0; i<num_vertices; ++i)		//copy coordinates to vertices
			{
				Vertex& v = m_vertices[i];
				v.id() = i;

				unsigned shift = 3*i;
				v.x() = p[shift];
				v.y() = p[shift + 1];
				v.z() = p[shift + 2];
			}

			m_faces.resize(num_faces);
			for(unsigned i=0; i<num_faces; ++i)		//copy adjacent vertices to polygons/faces
			{
				Face& f = m_faces[i];
				f.id() = i;
				f.adjacent_vertices().set_allocation(allocate_pointers(3),3);	//allocate three units of memory

				unsigned shift = 3*i;
				for(unsigned j=0; j<3; ++j)
				{
					unsigned vertex_index = tri[shift + j];
					assert(vertex_index < num_vertices);
					f.adjacent_vertices()[j] = &m_vertices[vertex_index];
				}
			}

			build_adjacencies();	//build the structure of the mesh
		}
	}

	template<class Points, class Faces>
	void initialize_mesh_data(Points& p, Faces& tri)		//build mesh from regular point-triangle representation
	{
		assert(p.size() % 3 == 0);
		unsigned const num_vertices = p.size() / 3;
		assert(tri.size() % 3 == 0);
		unsigned const num_faces = tri.size() / 3; 

		initialize_mesh_data(num_vertices, p, num_faces, tri);
	}

	std::vector<Vertex>& vertices(){return m_vertices;};
	std::vector<Edge>& edges(){return m_edges;};
	std::vector<Face>& faces(){return m_faces;};

	unsigned closest_vertices(SurfacePoint* p, 
								 std::vector<vertex_pointer>* storage = NULL);		//list vertices closest to the point

private:

	void build_adjacencies();		//build internal structure of the mesh
	bool verify();					//verifies connectivity of the mesh and prints some debug info

	typedef void* void_pointer;
	void_pointer allocate_pointers(unsigned n) 
	{
		return m_pointer_allocator.allocate(n); 
	}

	std::vector<Vertex> m_vertices;
	std::vector<Edge> m_edges;
	std::vector<Face> m_faces;

	SimlpeMemoryAllocator<void_pointer> m_pointer_allocator;	//fast memory allocating for Face/Vertex/Edge cross-references
};

void fill_surface_point_structure(geodesic::SurfacePoint* point, 
								  double* data, 
								  geodesic::Mesh* mesh);

void fill_surface_point_double(geodesic::SurfacePoint* point, 
							   double* data, 
							   long mesh_id);
} //geodesic

#endif	
