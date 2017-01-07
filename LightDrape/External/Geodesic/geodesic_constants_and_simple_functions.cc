#include "geodesic_constants_and_simple_functions.hh"
namespace geodesic{
	double geodesic::cos_from_edges(double const a,			//compute the cosine of the angle given the lengths of the edges
								 double const b,
								 double const c)
	{
		assert(a>1e-50);
		assert(b>1e-50);
		assert(c>1e-50);

		double result = (b*b + c*c - a*a)/(2.0*b*c);
		result = std::max(result, -1.0);
		return std::min(result, 1.0);
	}

	double geodesic::angle_from_edges(double const a,			//compute the cosine of the angle given the lengths of the edges
								   double const b,
								   double const c)
	{
		return acos(cos_from_edges(a,b,c));
	}

	template<class Points, class Faces>
	bool geodesic::read_mesh_from_file(char* filename,
									Points& points,
									Faces& faces)
	{
		std::ifstream file(filename);
		assert(file.is_open());
		if(!file.is_open()) return false;

		unsigned num_points;
		file >> num_points;
		assert(num_points>=3);

		unsigned num_faces;
		file >> num_faces;

		points.resize(num_points*3);
		for(typename Points::iterator i=points.begin(); i!=points.end(); ++i)
		{
			file >> *i;
		}

		faces.resize(num_faces*3);
		for(typename Faces::iterator i=faces.begin(); i!=faces.end(); ++i)
		{
			file >> *i;
		}
		file.close();

		return true;
	}
}