#include "geodesic_algorithm_exact_elements.hh"


void geodesic::Interval::find_closest_point(double const rs, 
										 double const hs,
										 double& r, 
										 double& d_out)			//find the point on the interval that is closest to the point (alpha, s)
{
	if(m_d == GEODESIC_INF)
	{
		r = GEODESIC_INF;
		d_out = GEODESIC_INF;
		return;
	}

	double hc = -m_pseudo_y;
	double rc = m_pseudo_x;
	double end = stop();

	double local_epsilon = SMALLEST_INTERVAL_RATIO*m_edge->length();
	if(std::abs(hs+hc) < local_epsilon)
	{
		if(rs<=m_start)
		{
			r = m_start;
			d_out = signal(m_start) + std::abs(rs - m_start);
		}
		else if(rs>=end)
		{
			r = end;
			d_out = signal(end) + fabs(end - rs);
		}
		else
		{
			r = rs;
			d_out = signal(rs);
		}
	}
	else
	{
		double ri = (rs*hc + hs*rc)/(hs+hc);

		if(ri<m_start)
		{
			r = m_start;
			d_out = signal(m_start) + hypotenuse(m_start - rs, hs);
		}
		else if(ri>end)
		{
			r = end;
			d_out = signal(end) + hypotenuse(end - rs, hs);
		}
		else
		{
			r = ri;
			d_out = m_d + hypotenuse(rc - rs, hc + hs);
		}
	}
}


void geodesic::Interval::initialize(edge_pointer edge, 
								 SurfacePoint* source,		
								 unsigned source_index)
{
	m_next = NULL;
	//m_geodesic_previous = NULL;	
	m_direction = UNDEFINED_DIRECTION;
	m_edge = edge;
	m_source_index = source_index;

	m_start = 0.0;
	//m_stop = edge->length();
	if(!source)
	{
		m_d = GEODESIC_INF;
		m_min = GEODESIC_INF;
		return;
	}
	m_d = 0;

	if(source->base_element()->type() == VERTEX)
	{
		if(source->base_element()->id() == edge->v0()->id())
		{
			m_pseudo_x = 0.0;
			m_pseudo_y = 0.0;
			m_min = 0.0;
			return;
		}
		else if(source->base_element()->id() == edge->v1()->id())
		{
			m_pseudo_x = stop();
			m_pseudo_y = 0.0;
			m_min = 0.0;
			return;
		}
	}

	edge->local_coordinates(source, m_pseudo_x, m_pseudo_y);
	m_pseudo_y = -m_pseudo_y;

	compute_min_distance(stop());
} 
