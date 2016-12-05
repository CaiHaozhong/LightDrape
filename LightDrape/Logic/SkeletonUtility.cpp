#include "SkeletonUtility.h"
#include <cmath>


SkeletonUtility::SkeletonUtility()
{
}

SkeletonUtility::~SkeletonUtility()
{
}

void SkeletonUtility::write( const Skeleton& skeleton, const char* file )
{
	size_t nodeCount = skeleton.nodeCount();
	size_t edgeCount = skeleton.edgeCount();
	typedef SkeletonNode::Point Point;
	std::ofstream out(file);
	out << nodeCount << "\n";
	for(size_t i = 0; i < nodeCount; i++){
		SkeletonNode* node = skeleton.nodeAt(i);
		float* point = node->point.values_;
		float* delta = node->delta.values_;
		Skeleton::IndexList& correspondanceList = node->correspondanceIndices;
		size_t correspondanceCount = correspondanceList.size();
		out << "point "
			<< point[0] << ' ' << point[1] << ' ' << point[2] 
			<< " delta "
			<< delta[0] << ' ' << delta[1] << ' ' << delta[2]
			<< " correspondance "	
		    << correspondanceCount << ' ';
		for (size_t j = 0; j < correspondanceCount; j++)
		{
			out << correspondanceList.at(j) << ' ';
		}
		out << "\n";	
	}

	out << edgeCount << "\n";

	for(size_t i = 0; i < edgeCount; i++){
		SkeletonEdge* edge = skeleton.edgeAt(i);
		out << edge->sourceVertex << ' ' << edge->targetVertex << "\n";
	}

	out.close();
}

void SkeletonUtility::read( Skeleton& skeleton, const char* file )
{
	skeleton.clear();
	std::ifstream input(file);
	int numVertices, numEdges, correspondance, correspondanceCount;
	std::string ignore;
	input >> numVertices;
	while(numVertices--){
		SkeletonNode* node = new SkeletonNode;
		input >> ignore
			  >> node->point.values_[0] >> node->point.values_[1] >> node->point.values_[2]
		      >> ignore
			  >> node->delta.values_[0] >> node->delta.values_[1] >> node->delta.values_[2]
			  >> ignore
		      >> correspondanceCount;
		while (correspondanceCount--){
			input >> correspondance;
			node->correspondanceIndices.push_back(correspondance);
		}
		skeleton.addNode(node);
	}
	input >> numEdges;
	while(numEdges--){
		SkeletonEdge* edge = new SkeletonEdge;
		input >> edge->sourceVertex >> edge->targetVertex;
		skeleton.addEdge(edge);
	}
	input.close();
}

void SkeletonUtility::readIntoContainer( const char* file )
{
	Skeleton skeleton;
	read(skeleton,file);
	globalSkeletonContainer.pushSkeleton(skeleton);
}

void SkeletonUtility::skeletonMatch( Skeleton& human, Skeleton& cloth )
{
	std::vector<size_t>& humanLeftHandVertices = human.getLeftHandVerticesRef();
	std::vector<size_t>& clothLeftHandVertices = cloth.getLeftHandVerticesRef();
	skeletonMatch(human, cloth, humanLeftHandVertices, clothLeftHandVertices);

	std::vector<size_t>& humanRightHandVertices = human.getRightHandVerticesRef();
	std::vector<size_t>& clothRightHandVertices = cloth.getRightHandVerticesRef();
	skeletonMatch(human, cloth, humanRightHandVertices, clothRightHandVertices);
}

void SkeletonUtility::skeletonMatch( Skeleton& human, Skeleton& cloth, std::vector<size_t>& humanHandVertices, std::vector<size_t>& clothHandVertices )
{
	typedef SkeletonNode::Point Point;
	float handLen = 0;
	for(int i = 0; i < clothHandVertices.size()-1; i++)
	{
		handLen += dis(cloth, clothHandVertices.at(i), clothHandVertices.at(i+1));
	}
	int slice = 20;
	float interval = handLen / slice;
	std::vector<Point> interpolatePointList;
	float curDis = 0;
	float intervalAdd = interval;
	for (int i = 0; i < humanHandVertices.size()-1 && interpolatePointList.size() <= slice; i++)
	{
		size_t from = humanHandVertices.at(i);
		size_t to = humanHandVertices.at(i+1);
		float fromValue = curDis;
		curDis += dis(human, from, to);		
		while(curDis > intervalAdd)
		{
			Point fromPoint = human.nodeAt(from)->point;
			Point toPoint = human.nodeAt(to)->point;
			interpolatePointList.push_back(interpolate(fromValue, curDis, fromPoint, toPoint,intervalAdd));
			if(interpolatePointList.size() > slice)
				break;
			intervalAdd += interval;
		}
	}

	Point clothNeck = cloth.nodeAt(cloth.getNeckIndex())->point;
	Point humanNeck = human.nodeAt(human.getNeckIndex())->point;
	double dx = clothNeck.values_[0] - humanNeck.values_[0];
	double dy = clothNeck.values_[1] - humanNeck.values_[1];
	double dz = clothNeck.values_[2] - humanNeck.values_[2];
	for (int i = 0; i < interpolatePointList.size(); i++)
	{
		Point p = interpolatePointList[i];
		interpolatePointList[i] = Point(p.values_[0] + dx, p.values_[1] + dy, p.values_[2] + dz);
	}

	//clothHandVertices.insert(clothHandVertices.begin(), cloth.mNeckIndex);
	interpolatePointList.insert(interpolatePointList.begin(), cloth.nodeAt(cloth.getNeckIndex())->point);
	float fromValue = 0;
	intervalAdd = interval + dis(cloth, cloth.getNeckIndex(), clothHandVertices.at(0));
	int curClothNode = 0;
	curDis = dis(cloth, cloth.getNeckIndex(), clothHandVertices.at(curClothNode));
	for (int i = 1; i < interpolatePointList.size() && curClothNode < clothHandVertices.size(); i++)
	{
		while (curDis <= intervalAdd)
		{
			Point fromPoint = interpolatePointList.at(i - 1);
			Point toPoint = interpolatePointList.at(i);
			Point targetPoint = interpolate(fromValue, intervalAdd, fromPoint, toPoint, curDis);
			cloth.nodeAt(clothHandVertices.at(curClothNode))->delta = pointSub(targetPoint,cloth.nodeAt(clothHandVertices.at(curClothNode))->point);
			curClothNode++;
			if(curClothNode < clothHandVertices.size())
				curDis += dis(cloth, clothHandVertices.at(curClothNode-1), clothHandVertices.at(curClothNode));
			else
			{
				break;
			}
		}
		fromValue = intervalAdd;
		intervalAdd += interval;
	}
}

float SkeletonUtility::dis( Skeleton& skeleton, const size_t from, const size_t to )
{
	SkeletonNode::Point fromPoint = skeleton.nodeAt(from)->point;
	SkeletonNode::Point toPoint = skeleton.nodeAt(to)->point;
	double dx = fromPoint.values_[0] - toPoint.values_[0];
	double dy = fromPoint.values_[1] - toPoint.values_[1];
	double dz = fromPoint.values_[2] - toPoint.values_[2];
	return sqrt(dx * dx + dy * dy + dz * dz);
}

SkeletonNode::Point SkeletonUtility::interpolate( float from, float to, SkeletonNode::Point& fromPoint, SkeletonNode::Point& toPoint, float target )
{
	double dx = toPoint.values_[0] - fromPoint.values_[0];
	double dy = toPoint.values_[1] - fromPoint.values_[1];
	double dz = toPoint.values_[2] - fromPoint.values_[2];

	float scale = (target - from)/ (to - from);

	return SkeletonNode::Point(fromPoint.values_[0] + scale * dx, fromPoint.values_[1] + scale * dy, fromPoint.values_[2] + scale * dz);
}

SkeletonNode::Point SkeletonUtility::pointSub( const SkeletonNode::Point& a, const SkeletonNode::Point& b )
{
	return SkeletonNode::Point(a.values_[0] - b.values_[0], a.values_[1] - b.values_[1], a.values_[2] - b.values_[2]);
}

void SkeletonUtility::recomputeCorrepspondence( Skeleton& skeleton, Mesh& mesh, int numberOfCorrepondencePoint )
{
	std::vector<Point_3> points;
	for(Mesh::VertexIter it = mesh.vertices_begin(); it != mesh.vertices_end(); it++)
	{
		Mesh::Point& openMeshPoint = mesh.point(*it);
		Point_3 cgalPoint(openMeshPoint.values_[0],openMeshPoint.values_[1],openMeshPoint.values_[2]);
		points.push_back(cgalPoint);
	}
	KNNSHelper knnsHelper(points);		
	for(int i = 0; i < skeleton.nodeCount(); i++)
	{
		SkeletonNode* skeletonNode = skeleton.nodeAt(i);
		skeletonNode->correspondanceIndices.clear();
		SkeletonNode::Point skeletonPoint = skeletonNode->point;
		knnsHelper.kNeighborSearch(Point_3(skeletonPoint.values_[0],skeletonPoint.values_[1],skeletonPoint.values_[2]),numberOfCorrepondencePoint);
		for(KNNSHelper::KNNSHelperIterator it = knnsHelper.begin(); it != knnsHelper.end(); it++)
		{
			int neighbor = knnsHelper.getIndex(it);
			skeletonNode->correspondanceIndices.push_back(neighbor);
		}
	}
}