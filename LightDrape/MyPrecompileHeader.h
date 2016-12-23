/* For Skeletonization */
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/graph_traits_Surface_mesh.h>
#include <CGAL/boost/graph/properties_Surface_mesh.h>
#include <CGAL/Mean_curvature_flow_skeletonization.h>
typedef CGAL::Simple_cartesian<double>                        Simple_cartesian_Kernel;
typedef CGAL::Surface_mesh<Simple_cartesian_Kernel::Point_3>                   CGAL_Surface_mesh;
typedef CGAL::Mean_curvature_flow_skeletonization<CGAL_Surface_mesh> Skeletonization;


/* For Geodesic */
// #include <CGAL/Polyhedron_3.h>
/*#include <CGAL/Polyhedron_items_with_id_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh_shortest_path.h>
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
#include <CGAL/boost/graph/iterator.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel EPICKernel;
typedef CGAL::Polyhedron_3<EPICKernel, CGAL::Polyhedron_items_with_id_3> CGAL_Polyhedron_3;
typedef CGAL::Surface_mesh_shortest_path_traits<EPICKernel, CGAL_Polyhedron_3> ShortPathTraits;
typedef CGAL::Surface_mesh_shortest_path<ShortPathTraits> Surface_mesh_shortest_path;
typedef boost::graph_traits<CGAL_Polyhedron_3> CGAL_Polyhedron_Graph_traits;
typedef CGAL_Polyhedron_Graph_traits::vertex_iterator shortest_path_vertex_iterator;
typedef CGAL_Polyhedron_Graph_traits::face_iterator shortest_path_face_iterator;*/