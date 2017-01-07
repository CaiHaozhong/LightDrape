#include "geodesic_algorithm_base.hh"

double geodesic::length( std::vector<SurfacePoint>& path )
{
	double length = 0;
	if(!path.empty())
	{
		for(unsigned i=0; i<path.size()-1; ++i)
		{
			length += path[i].distance(&path[i+1]);
		}
	}
	return length;
}

void geodesic::print_info_about_path( std::vector<SurfacePoint>& path )
{
	std::cout << "number of the points in the path = " << path.size()
		<< ", length of the path = " << length(path) 
		<< std::endl;
}

std::string geodesic::GeodesicAlgorithmBase::name()
{
	switch(m_type)
	{
	case EXACT:
		return "exact";
	case DIJKSTRA:
		return "dijkstra";
	case SUBDIVISION:
		return "subdivision";
	default:
	case UNDEFINED_ALGORITHM:
		return "undefined";
	}
}
void geodesic::GeodesicAlgorithmBase::geodesic(SurfacePoint& source,
									 SurfacePoint& destination,
									 std::vector<SurfacePoint>& path) //lazy people can find geodesic path with one function call
{
	std::vector<SurfacePoint> sources(1, source);
	std::vector<SurfacePoint> stop_points(1, destination);
	double const max_propagation_distance = GEODESIC_INF;

	propagate(sources, 
		max_propagation_distance,
		&stop_points);

	trace_back(destination, path);
}

void geodesic::GeodesicAlgorithmBase::geodesic(std::vector<SurfacePoint>& sources,
									 std::vector<SurfacePoint>& destinations,
									 std::vector<std::vector<SurfacePoint> >& paths) //lazy people can find geodesic paths with one function call
{
	double const max_propagation_distance = GEODESIC_INF;

	propagate(sources, 
		max_propagation_distance,
		&destinations);		//we use desinations as stop points

	paths.resize(destinations.size());

	for(unsigned i=0; i<paths.size(); ++i)
	{
		trace_back(destinations[i], paths[i]);
	}
}

void geodesic::GeodesicAlgorithmBase::set_stop_conditions(std::vector<SurfacePoint>* stop_points, 
												double stop_distance)
{
	m_max_propagation_distance = stop_distance;

	if(!stop_points)
	{
		m_stop_vertices.clear();
		return;
	}

	m_stop_vertices.resize(stop_points->size());

	std::vector<vertex_pointer> possible_vertices;
	for(unsigned i = 0; i < stop_points->size(); ++i)
	{
		SurfacePoint* point = &(*stop_points)[i];

		possible_vertices.clear();
		m_mesh->closest_vertices(point, &possible_vertices);

		vertex_pointer closest_vertex = NULL;
		double min_distance = 1e100;
		for(unsigned j = 0; j < possible_vertices.size(); ++j)
		{
			double distance = point->distance(possible_vertices[j]);
			if(distance < min_distance)
			{
				min_distance = distance;
				closest_vertex = possible_vertices[j];
			}
		}
		assert(closest_vertex);

		m_stop_vertices[i].first = closest_vertex;
		m_stop_vertices[i].second = min_distance;
	}
}