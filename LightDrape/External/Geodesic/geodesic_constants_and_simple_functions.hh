//Copyright (C) 2008 Danil Kirsanov, MIT License
#ifndef GEODESIC_CONSTANTS_20071231
#define GEODESIC_CONSTANTS_20071231

// some constants and simple math functions

#include <assert.h>
#include <math.h>
#include <limits>
#include <fstream>

namespace geodesic{

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//double const GEODESIC_INF = std::numeric_limits<double>::max();
double const GEODESIC_INF = 1e100;

//in order to avoid numerical problems with "infinitely small" intervals,
//we drop all the intervals smaller than SMALLEST_INTERVAL_RATIO*edge_length
double const SMALLEST_INTERVAL_RATIO = 1e-6;		
//double const SMALL_EPSILON = 1e-10;


double cos_from_edges(double const a,			//compute the cosine of the angle given the lengths of the edges
							 double const b,
							 double const c);

double angle_from_edges(double const a,			//compute the cosine of the angle given the lengths of the edges
							   double const b,
							   double const c);

template<class Points, class Faces>
inline bool read_mesh_from_file(char* filename,
								Points& points,
								Faces& faces);
} //geodesic

#endif	//GEODESIC_CONSTANTS_20071231
