#include "MeshSkeletonization.h"
#include "WatertightMesh.h"

MeshSkeletonization::MeshSkeletonization(void)
{
	m_quality_speed_tradeoff = 0.1;
}


MeshSkeletonization::~MeshSkeletonization(void)
{
}

void MeshSkeletonization::skeletonize( WatertightMesh_ mesh )
{
	CGAL_Surface_mesh triangle_mesh;

	std::for_each(mesh->vertices_begin(), mesh->vertices_end(), [&](const Mesh::VertexHandle vIndex){
		Vec3d meshPoint = mesh->point(vIndex);
		Simple_cartesian_Kernel::Point_3 p(meshPoint.values_[0], meshPoint.values_[1], meshPoint.values_[2]);
		triangle_mesh.add_vertex(p);
	});

	typedef CGAL_Surface_mesh::size_type size_type;
	std::for_each(mesh->faces_begin(), mesh->faces_end(), [&](const Mesh::FaceHandle fIndex){
		std::vector<size_type> face;
		for(Mesh::ConstFaceVertexIter faceVertexIter = mesh->cfv_begin(fIndex); faceVertexIter.is_valid(); faceVertexIter++){
			face.push_back(faceVertexIter->idx());
		}
		assert(face.size() == 3);
		triangle_mesh.add_face(face);
	});

	Skeletonization mcs = Skeletonization(triangle_mesh);
	mcs.set_quality_speed_tradeoff(m_quality_speed_tradeoff);
	mcs.contract_until_convergence();
	Skeletonization::Skeleton cgalSkeleton;
	mcs.convert_to_skeleton(cgalSkeleton);

	/* Set Skeleton */
	Skeleton_ skeleton  = makeSkeleton(cgalSkeleton);	
	mesh->setSkeleton(skeleton);

	/** Set vertex property of skeleton node **/
	IntProperty_ vertexPropertySKN = smartNew(IntProperty);
	mesh->registerProperty(vertexPropertySKN);
	const Skeleton::NodeList& nodeList = skeleton->getNodeList();
	for(size_t i = 0; i < nodeList.size(); i++){
		std::vector<size_t>& corsVertices = nodeList[i]->correspondanceIndices;
		for(size_t j = 0; j < corsVertices.size(); j++){
			vertexPropertySKN->set(corsVertices[j], i);
		}
	}
	/* Set Property */
	mesh->setVertexPropertySKN(vertexPropertySKN);

	std::cout << "Number of vertices of the skeleton: " << boost::num_vertices(cgalSkeleton) << "\n";
	std::cout << "Number of edges of the skeleton: " << boost::num_edges(cgalSkeleton) << "\n";
}

Skeleton_ MeshSkeletonization::makeSkeleton( CGALSkeleton& cgalSkeleton )
{
	typedef CGALSkeleton::edge_descriptor edge_descriptor;
	typedef CGALSkeleton::vertex_descriptor vertex_descriptor;
	Skeleton_ ret = smartNew(Skeleton);
	BOOST_FOREACH(vertex_descriptor v, boost::vertices(cgalSkeleton)){
		SkeletonNode_ node = smartNew(SkeletonNode);
		auto skeletonizationPoint = cgalSkeleton[v].point;
		node->point = Vec3d(skeletonizationPoint.x(), skeletonizationPoint.y(), skeletonizationPoint.z());
		std::vector<CGAL::SM_Vertex_index> cors = cgalSkeleton[v].vertices;
		for(size_t i = 0; i < cors.size(); i++){
			node->correspondanceIndices.push_back(cors[i]);
		}
		ret->addNode(node);
	}
	BOOST_FOREACH(edge_descriptor e, boost::edges(cgalSkeleton)){
		SkeletonEdge_ edge = std::make_shared<SkeletonEdge>(boost::source(e,cgalSkeleton), boost::target(e,cgalSkeleton));
		ret->addEdge(edge);			
	}
	return ret;
}

void MeshSkeletonization::set_quality_speed_tradeoff( double val )
{
	m_quality_speed_tradeoff = val;
}
