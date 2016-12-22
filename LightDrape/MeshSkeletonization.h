#pragma once
#include "MyPrecompileHeader.h"
#include "WatertightMesh.h"
/* 对Mesh进行骨骼提取，将提取到的骨骼传入该Mesh对象，将“顶点->骨骼节点”的
 * 属性传入该Mesh对象
 * 要求Mesh为WatertightMesh
 */
class MeshSkeletonization
{
public:
	MeshSkeletonization(void);
	~MeshSkeletonization(void);
	void skeletonize(WatertightMesh_ mesh){
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
			assert(face.size() == 3, "not triangle face");
			triangle_mesh.add_face(face);
		});

		Skeletonization mcs = Skeletonization(triangle_mesh);
		mcs.contract_until_convergence();
		Skeletonization::Skeleton cgalSkeleton;
		mcs.convert_to_skeleton(cgalSkeleton);

		/* Set Skeleton */
		Skeleton_ skeleton  = makeSkeleton(cgalSkeleton);	
		mesh->setSkeleton(skeleton);

		/** Set vertex property on skeleton node **/
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

private:
	/* 将CgalSkeleton转成Skeleton */
	Skeleton_ makeSkeleton( Skeletonization::Skeleton& cgalSkeleton ){
		typedef Skeletonization::Skeleton::edge_descriptor edge_descriptor;
		typedef Skeletonization::Skeleton::vertex_descriptor vertex_descriptor;
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
};

