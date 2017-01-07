//Copyright (C) 2008 Danil Kirsanov, MIT License
#ifndef GEODESIC_ALGORITHM_EXACT_20071231
#define GEODESIC_ALGORITHM_EXACT_20071231

#include "geodesic_memory.hh"
#include "geodesic_algorithm_base.hh"
#include "geodesic_algorithm_exact_elements.hh"
#include <vector>
#include <cmath>
#include <assert.h>
#include <set>

namespace geodesic{

class GeodesicAlgorithmExact : public GeodesicAlgorithmBase
{
public:
	GeodesicAlgorithmExact(geodesic::Mesh* mesh):
	  	GeodesicAlgorithmBase(mesh),
		m_memory_allocator(mesh->edges().size(), mesh->edges().size()),
		m_edge_interval_lists(mesh->edges().size())
	{
		m_type = EXACT;

		for(unsigned i=0; i<m_edge_interval_lists.size(); ++i)
		{
			m_edge_interval_lists[i].initialize(&mesh->edges()[i]);
		}
	};	

	~GeodesicAlgorithmExact(){};

	void propagate(std::vector<SurfacePoint>& sources,
   				   double max_propagation_distance = GEODESIC_INF,			//propagation algorithm stops after reaching the certain distance from the source
				   std::vector<SurfacePoint>* stop_points = NULL); //or after ensuring that all the stop_points are covered

	void trace_back(SurfacePoint& destination,		//trace back piecewise-linear path
					std::vector<SurfacePoint>& path);

	unsigned best_source(SurfacePoint& point,			//quickly find what source this point belongs to and what is the distance to this source
		double& best_source_distance); 

	void print_statistics();

private:
	typedef std::set<interval_pointer, Interval> IntervalQueue;

	void update_list_and_queue(list_pointer list,
							   IntervalWithStop* candidates,	//up to two candidates
							   unsigned num_candidates);

	unsigned compute_propagated_parameters(double pseudo_x, 
											double pseudo_y, 
											double d,		//parameters of the interval
											double start, 
											double end,		//start/end of the interval
											double alpha,	//corner angle
											double L,		//length of the new edge
											bool first_interval,		//if it is the first interval on the edge
											bool last_interval,
											bool turn_left,
											bool turn_right,
											IntervalWithStop* candidates);		//if it is the last interval on the edge

	void construct_propagated_intervals(bool invert, 
									  edge_pointer edge, 
									  face_pointer face,		//constructs iNew from the rest of the data
									  IntervalWithStop* candidates,
									  unsigned& num_candidates,
									  interval_pointer source_interval);

	double compute_positive_intersection(double start,
										 double pseudo_x,
										 double pseudo_y,
										 double sin_alpha,
										 double cos_alpha);		//used in construct_propagated_intervals

	unsigned intersect_intervals(interval_pointer zero, 
								    IntervalWithStop* one);			//intersecting two intervals with up to three intervals in the end

	interval_pointer best_first_interval(SurfacePoint& point, 
										double& best_total_distance, 
										double& best_interval_position,
										unsigned& best_source_index);

	bool check_stop_conditions(unsigned& index);

	void clear()
	{
		m_memory_allocator.clear();
		m_queue.clear();
		for(unsigned i=0; i<m_edge_interval_lists.size(); ++i)
		{
			m_edge_interval_lists[i].clear();
		}
		m_propagation_distance_stopped = GEODESIC_INF;
	};

	list_pointer interval_list(edge_pointer e)
	{
		return &m_edge_interval_lists[e->id()];
	};

	void set_sources(std::vector<SurfacePoint>& sources)
	{
		m_sources.initialize(sources);
	}

	void initialize_propagation_data();		

	void list_edges_visible_from_source(MeshElementBase* p,
										std::vector<edge_pointer>& storage); //used in initialization

	long visible_from_source(SurfacePoint& point);	//used in backtracing

	void best_point_on_the_edge_set(SurfacePoint& point, 
									std::vector<edge_pointer> const& storage,
									interval_pointer& best_interval,
									double& best_total_distance,
									double& best_interval_position);

	void possible_traceback_edges(SurfacePoint& point, 
								  std::vector<edge_pointer>& storage);

	bool erase_from_queue(interval_pointer p);

	IntervalQueue m_queue;	//interval queue

	MemoryAllocator<Interval> m_memory_allocator;			//quickly allocate and deallocate intervals 
	std::vector<IntervalList> m_edge_interval_lists;		//every edge has its interval data 

	enum MapType {OLD, NEW};		//used for interval intersection
	MapType map[5];		
	double start[6];
	interval_pointer i_new[5];

	unsigned m_queue_max_size;			//used for statistics
	unsigned m_iterations;			//used for statistics

	SortedSources m_sources;
};

}		//geodesic

#endif //GEODESIC_ALGORITHM_EXACT_20071231
