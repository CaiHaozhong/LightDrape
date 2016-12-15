#include "MyPrecompileHeader.h"
// #include <CGAL/Polyhedron_3.h>
// #include <CGAL/Polyhedron_items_with_id_3.h>
// #include <CGAL/IO/Polyhedron_iostream.h>
// #include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
// #include <CGAL/Simple_cartesian.h>
// #include <CGAL/extract_mean_curvature_flow_skeleton.h>
// #include <CGAL/mesh_segmentation.h>
#include <fstream>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <io.h>
#include <direct.h>  
typedef OpenMesh::TriMesh_ArrayKernelT<> _Mesh;
typedef _Mesh Mesh;
typedef CGAL::Simple_cartesian<double>                               Kernel;
typedef Kernel::Point_3                                              Point;
typedef CGAL::Polyhedron_3<Kernel, CGAL::Polyhedron_items_with_id_3> Polyhedron;
typedef boost::graph_traits<Polyhedron>::vertex_descriptor           vertex_descriptor;
typedef boost::graph_traits<Polyhedron>::halfedge_descriptor         halfedge_descriptor;
typedef boost::graph_traits<Polyhedron>::face_descriptor             face_descriptor;
typedef CGAL::Mean_curvature_flow_skeletonization<Polyhedron>        Skeletonization;
typedef Skeletonization::Skeleton                                    Skeleton;
typedef Skeleton::vertex_descriptor                                  Skeleton_vertex;
// Property map associating a facet with an integer as id to an
// element in a vector stored internally
template<class ValueType>
struct Facet_with_id_pmap
	: public boost::put_get_helper<ValueType&,
	Facet_with_id_pmap<ValueType> >
{
	typedef face_descriptor key_type;
	typedef ValueType value_type;
	typedef value_type& reference;
	typedef boost::lvalue_property_map_tag category;
	Facet_with_id_pmap(
		std::vector<ValueType>& internal_vector
		) : internal_vector(internal_vector) { }
	reference operator[](key_type key) const
	{ return internal_vector[key->id()]; }
private:
	std::vector<ValueType>& internal_vector;
};
int main(int argc, char* argv[])
{
	Mesh m;
	OpenMesh::IO::read_mesh(m, "E:/Project/LightDrape/data/scapecomp_watertight/mesh002.obj");
	size_t maxyi = 0;
	float maxy = -1000000;
	size_t cur = 0;
	for(Mesh::VertexIter it = m.vertices_begin(); it != m.vertices_end(); it++){
		Mesh::Point& p = m.point(*it);
		if(p.values_[1] > maxy){
			maxy = p.values_[1];
			maxyi = cur;
		}
		cur ++;
	}
	std::cout << maxyi << std::endl;
	getchar();
	std::ifstream input((argc>1)?argv[1]:"E:/Project/LightDrape/data/scapecomp_watertight/mesh003remesh.off");
	Polyhedron tmesh;
	input >> tmesh;
	// extract the skeleton
	Skeleton skeleton;
	std::cout << "begin extract\n";
	CGAL::extract_mean_curvature_flow_skeleton(tmesh, skeleton);
	std::cout << "end extract\n";
	// init the polyhedron simplex indices
	CGAL::set_halfedgeds_items_id(tmesh);
	//for each input vertex compute its distance to the skeleton
	std::vector<double> distances(num_vertices(tmesh));
	BOOST_FOREACH(Skeleton_vertex v, vertices(skeleton) )
	{
		const Point& skel_pt = skeleton[v].point;
		BOOST_FOREACH(vertex_descriptor mesh_v, skeleton[v].vertices)
		{
			const Point& mesh_pt = mesh_v->point();
			distances[mesh_v->id()] = std::sqrt(CGAL::squared_distance(skel_pt, mesh_pt));
		}
	}
	// create a property-map for sdf values
	std::vector<double> sdf_values( num_faces(tmesh) );
	Facet_with_id_pmap<double> sdf_property_map(sdf_values);
	// compute sdf values with skeleton
	BOOST_FOREACH(face_descriptor f, faces(tmesh))
	{
		double dist = 0;
		BOOST_FOREACH(halfedge_descriptor hd, halfedges_around_face(halfedge(f, tmesh), tmesh))
			dist+=distances[target(hd, tmesh)->id()];
		sdf_property_map[f] = dist / 3.;
	}
	// post-process the sdf values
	CGAL::sdf_values_postprocessing(tmesh, sdf_property_map);
	// create a property-map for segment-ids (it is an adaptor for this case)
	std::vector<std::size_t> segment_ids( num_faces(tmesh) );
	Facet_with_id_pmap<std::size_t> segment_property_map(segment_ids);
	// segment the mesh using default parameters
	std::size_t numOfSegment = CGAL::segmentation_from_sdf_values(tmesh, sdf_property_map, segment_property_map);
	std::cout << "Number of segments: " << numOfSegment <<"\n";
	
	/** write segments to individual objs **/
	std::vector<Mesh> meshList;
	meshList.resize(numOfSegment);
	for(int i = 0; i < numOfSegment; i++){
		for(auto it = tmesh.points_begin(); it != tmesh.points_end(); it++){
			Polyhedron::Point_3 p = *it;
			meshList[i].add_vertex(Mesh::Point(p.x(), p.y(), p.z()));
		}
	}
	BOOST_FOREACH(face_descriptor f, faces(tmesh))
	{
		double dist = 0;
		std::vector<Mesh::VertexHandle> openMeshFace;		
		BOOST_FOREACH(vertex_descriptor vd, vertices_around_face(halfedge(f,tmesh), tmesh))
		{
			openMeshFace.push_back(Mesh::VertexHandle(vd->id()));
		}
		meshList[segment_property_map[f]].add_face(openMeshFace);
	}
	std::string outPath = std::string("E:\\Project\\LightDrape\\data\\segmentation\\scapecomp\\mesh003remesh\\");
	if(_access(outPath.c_str(), 0) == ENOENT){
		_mkdir(outPath.c_str());
	}
	for (int i = 0; i < meshList.size(); i++)
	{
		char file[20];
		sprintf(file, "segment%d.obj", i);
		OpenMesh::IO::write_mesh(meshList[i], outPath+std::string(file));
	}
	std::cout << "end" << std::endl;
	getchar();
	getchar();
	return 0;
}
