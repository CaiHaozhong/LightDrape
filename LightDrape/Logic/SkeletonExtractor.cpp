#include "SkeletonExtractor.h"

void SkeletonExtractor::extract(Mesh& mesh){
	Triangle_mesh triangle_mesh;
	
	std::for_each(mesh.vertices_begin(), mesh.vertices_end(), [&](const Mesh::VertexHandle vIndex){
		Mesh::Point meshPoint = mesh.point(vIndex);
		Kernel::Point_3 p(meshPoint.values_[0], meshPoint.values_[1], meshPoint.values_[2]);
		triangle_mesh.add_vertex(p);
	});

	typedef Triangle_mesh::size_type size_type;
	std::for_each(mesh.faces_begin(), mesh.faces_end(), [&](const Mesh::FaceHandle fIndex){
		std::vector<size_type> face;
		for(Mesh::ConstFaceVertexIter faceVertexIter = mesh.cfv_begin(fIndex); faceVertexIter.is_valid(); faceVertexIter++){
			face.push_back(faceVertexIter->idx());
		}
		assert(face.size() == 3, "not triangle face");
		triangle_mesh.add_face(face);
	});

	Skeletonization mcs(triangle_mesh);

	// 1. Contract the mesh by mean curvature flow.
	mcs.contract_geometry();

	// 2. Collapse short edges and split bad triangles.
	mcs.collapse_edges();
	mcs.split_faces();

	// 3. Fix degenerate vertices.
	mcs.detect_degeneracies();

	// Perform the above three steps in one iteration.
	mcs.contract();

	// Iteratively apply step 1 to 3 until convergence.
	mcs.contract_until_convergence();

	// Convert the contracted mesh into a curve skeleton and
	// get the correspondent surface points
	Skeletonization::Skeleton cgalSkeleton;
	mcs.convert_to_skeleton(cgalSkeleton);

	Skeleton* skeleton  = makeSkeleton(cgalSkeleton);	
	mesh.setSkeleton(skeleton);

// 	std::cout << "Number of vertices of the skeleton: " << boost::num_vertices(skeleton) << "\n";
// 	std::cout << "Number of edges of the skeleton: " << boost::num_edges(skeleton) << "\n";

	// Output skeleton points and the corresponding surface points
// 	std::ofstream output;
// 	output.open("correspondance.cgal");
// 	BOOST_FOREACH(Skeleton_vertex v, vertices(skeleton))
// 		BOOST_FOREACH(vertex_descriptor vd, skeleton[v].vertices)
// 		output << "2 " << skeleton[v].point << "  " << boost::get(CGAL::vertex_point, triangle_mesh, vd)  << "\n";
// 	output.close();

}

Skeleton* SkeletonExtractor::makeSkeleton( Skeletonization::Skeleton& cgalSkeleton )
{
	typedef Skeletonization::Skeleton::edge_descriptor edge_descriptor;
	typedef Skeletonization::Skeleton::vertex_descriptor vertex_descriptor;
	Skeleton* ret = new Skeleton();
	BOOST_FOREACH(vertex_descriptor v, boost::vertices(skeleton)){
		SkeletonNode* node = new SkeletonNode;
		auto skeletonizationPoint = skeleton[v].point;
		node->point = SkeletonNode::Point(skeletonizationPoint.x(), skeletonizationPoint.y(), skeletonizationPoint.z());
		node->correspondanceIndices = skeleton[v].correspondanceIndices;
		ret->addNode(node);
	}
	BOOST_FOREACH(edge_descriptor e, boost::edges(skeleton)){
		SkeletonEdge* edge = new SkeletonEdge(boost::source(e,skeleton), boost::target(e,skeleton));
		ret->addEdge(edge);			
	}
	return ret;
}