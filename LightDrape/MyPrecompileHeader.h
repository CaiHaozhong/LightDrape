#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/graph_traits_Surface_mesh.h>
#include <CGAL/boost/graph/properties_Surface_mesh.h>
#include <CGAL/Mean_curvature_flow_skeletonization.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/extract_mean_curvature_flow_skeleton.h>
#include <CGAL/mesh_segmentation.h>
typedef CGAL::Simple_cartesian<double>                        Simple_cartesian_Kernel;
typedef CGAL::Surface_mesh<Simple_cartesian_Kernel::Point_3>                   CGAL_Surface_mesh;
typedef CGAL::Mean_curvature_flow_skeletonization<CGAL_Surface_mesh> Skeletonization;
