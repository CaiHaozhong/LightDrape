#include "SkeletonExtractor.h"
#include <fstream>
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

	mcs = new Skeletonization(triangle_mesh);
	mcs->set_area_variation_factor(0.07);
	mcs->contract_until_convergence();
// 	// 1. Contract the mesh by mean curvature flow.
// 	mcs.contract_geometry();
// 
// 	// 2. Collapse short edges and split bad triangles.
// 	mcs.collapse_edges();
// 	mcs.split_faces();
// 
// 	// 3. Fix degenerate vertices.
// 	mcs.detect_degeneracies();
// 
// 	// Perform the above three steps in one iteration.
//	mcs.contract();

	// Iteratively apply step 1 to 3 until convergence.
	//mcs.contract_until_convergence();

	// Convert the contracted mesh into a curve skeleton and
	// get the correspondent surface points
	Skeletonization::Skeleton cgalSkeleton;
	mcs->convert_to_skeleton(cgalSkeleton);

	Skeleton* skeleton  = makeSkeleton(cgalSkeleton);	
	mesh.setSkeleton(skeleton);

 	std::cout << "Number of vertices of the skeleton: " << boost::num_vertices(cgalSkeleton) << "\n";
 	std::cout << "Number of edges of the skeleton: " << boost::num_edges(cgalSkeleton) << "\n";
}

Skeleton* SkeletonExtractor::makeSkeleton( Skeletonization::Skeleton& cgalSkeleton )
{
	typedef Skeletonization::Skeleton::edge_descriptor edge_descriptor;
	typedef Skeletonization::Skeleton::vertex_descriptor vertex_descriptor;
	Skeleton* ret = new Skeleton();
	BOOST_FOREACH(vertex_descriptor v, boost::vertices(cgalSkeleton)){
		SkeletonNode* node = new SkeletonNode;
		auto skeletonizationPoint = cgalSkeleton[v].point;
		node->point = SkeletonNode::Point(skeletonizationPoint.x(), skeletonizationPoint.y(), skeletonizationPoint.z());
		//node->correspondanceIndices = cgalSkeleton[v].correspondanceIndices;
		ret->addNode(node);
	}
	BOOST_FOREACH(edge_descriptor e, boost::edges(cgalSkeleton)){
		SkeletonEdge* edge = new SkeletonEdge(boost::source(e,cgalSkeleton), boost::target(e,cgalSkeleton));
		ret->addEdge(edge);			
	}
	return ret;
}

void SkeletonExtractor::dumpSkeleton(Skeleton& skeleton,  std::string file)
{
	std::ofstream out = std::ofstream(file);
	out << "# D:3 NV:" << skeleton.nodeCount() << " NE:" << skeleton.edgeCount() << "\n";
	Skeleton::NodeList nodes = skeleton.getNodeList();
	for(int i = 0; i < nodes.size(); i++){
		SkeletonNode* n = nodes.at(i);
		out << "v " << n->point.values_[0] << " " << n->point.values_[1] << " " << n->point.values_[2] << "\n";
	}
	Skeleton::EdgeList edges = skeleton.getEdgeList();
	for(int i = 0; i < edges.size(); i++){
		SkeletonEdge* e = edges.at(i);
		out << "e " << e->sourceVertex+1 << " " << e->targetVertex+1 << "\n";
	}
	out.close();
}

void SkeletonExtractor::dumpMesoSkeleton( std::string file )
{
	Skeletonization::Meso_skeleton& meso_skeleton = mcs->meso_skeleton();
	std::ofstream out = std::ofstream(file);
	out << "# D:3 NV:" << meso_skeleton.size_of_vertices() << " NE:" << 0 << "\n";
	for(auto pit = meso_skeleton.points_begin(); pit != meso_skeleton.points_end(); pit++){
		Skeletonization::Meso_skeleton::Point_3 p = *pit;
		out << "v " << p << "\n";
	}
	out.close();
}

SkeletonExtractor::SkeletonExtractor()
{
	mcs = nullptr;
}

SkeletonExtractor::~SkeletonExtractor()
{
	if(mcs != nullptr)
		delete mcs;
}
