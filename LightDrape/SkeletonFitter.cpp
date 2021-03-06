#include "SkeletonFitter.h"
#include "Region.h"
#include "WatertightMesh.h"
#include "VertexAlter.h"
#include "Config.h"

size_t ChooseLongBranch::decide( size_t prev, std::vector<size_t>& nextNodes )
{
	std::vector< std::pair<size_t, size_t> > nodeLens;
	size_t nodeCount = nextNodes.size();
	for(size_t i = 0; i < nodeCount; i++){
		nodeLens.push_back(std::make_pair(nextNodes[i], length(prev, nextNodes[i])));
	}
	typedef std::pair<size_t, size_t> NL;
	std::sort(nodeLens.begin(), nodeLens.end(), [](const NL& a, const NL& b){
		return a.second > b.second;
	});
	return nodeLens[0].first;
}

size_t ChooseLongBranch::length( size_t prev, size_t node )
{
	Skeleton::IndexList& neighbors = mSkeleton->getNeighbors(node);
	size_t ret = 1;
	size_t neiCount = neighbors.size();
	/* 只有一个邻居或有两个以上的邻居都视为终点 */
	while(neiCount == 2){
		for(size_t i = 0; i < 2; i++){
			size_t nei = neighbors[i];
			if(nei != prev){
				prev = node;
				node = nei;
				ret += 1;
			}
		}
		neighbors = mSkeleton->getNeighbors(node);
		neiCount = neighbors.size();
	}
	return ret;
}

size_t ChooseRadomBranch::decide( size_t prev, std::vector<size_t>& nextNodes )
{
	return nextNodes[0];
}

SkeletonFitter::SkeletonFitter(void)
{
}

SkeletonFitter::SkeletonFitter( Region_ garmentRegion ) : RegionFitter(garmentRegion)
{

}


SkeletonFitter::~SkeletonFitter(void)
{
}
void SkeletonFitter::getSortedSkeleton( Region_ region, std::vector<size_t>& sortedSkeNodes,
											 MultiNextNodeHandler_ multiNextNodeHandler)
{
	Skeleton_ skeleton = this->getSkeleton(region);
	std::set<size_t>& skeletonNodes = getSkeletonNodesInRegion(region);
	size_t cur = getStartSkeletonNode(region);
	bool curValid = true;
	size_t last = cur;
	while(curValid){
		sortedSkeNodes.push_back(cur);
		Skeleton::IndexList& neighbors = skeleton->getNeighbors(cur);
		size_t neighborCount = neighbors.size();		
		std::vector<size_t> nextNodes;
		for(size_t i = 0; i < neighborCount; i++){
			size_t nei = neighbors[i];
			if(skeletonNodes.find(nei) != skeletonNodes.end()
				&& nei != last){					
					nextNodes.push_back(nei);
			}
		}
		if(nextNodes.size() == 0){
			curValid = false;
		}
		else{
			size_t next = nextNodes[0];
			if(nextNodes.size() != 1){							
				next = multiNextNodeHandler->decide(cur, nextNodes);
				char msg[100];				
				sprintf(msg, "multi next node on %d, decide %d as next node", cur, next);
				PRINTLN(msg);
			}
			last = cur;
			cur = next;
		}
	}
}

void SkeletonFitter::fitSkeleton(std::vector< std::pair<size_t, Vec3d> >& garSkeTrans, 
									   std::vector< std::pair<size_t, std::pair<Vec3d, double> > >& garSkeRotate,
									   Region_ humanRegion)
{
	if( mGarmentRegion == nullptr || humanRegion == nullptr){
		PRINTLN("In SkeletonFitter, regions should not be null.");
		return;
	}
	Skeleton_ garSkeleton = this->getSkeleton(mGarmentRegion);
	Skeleton_ humSkeleton = this->getSkeleton(humanRegion);

	std::vector<size_t> garOrderedSkes;
	std::vector<size_t> humOrderSkes;
	getSortedSkeleton(mGarmentRegion, garOrderedSkes, std::make_shared<ChooseLongBranch>(garSkeleton));
	getSortedSkeleton(humanRegion, humOrderSkes, std::make_shared<ChooseLongBranch>(humSkeleton));
	if(mGarmentRegion->getName() == "leftsleeve"){
		std::cout << "garOrderedSkes: ";
		for(size_t k = 0; k < garOrderedSkes.size(); k++)
		{
			std::cout << garOrderedSkes[k] << " ";
		}
		std::cout << "\n";
	}
	if(humanRegion->getName() == "lefthand"){
		std::cout << "humOrderSkes: ";
		for(size_t k = 0; k < humOrderSkes.size(); k++)
		{
			std::cout << humOrderSkes[k] << " ";
		}
		std::cout << "\n";
	}
	size_t garStartSke = garOrderedSkes[0];
	size_t humStartSke = humOrderSkes[0];
	garSkeleton->dumpPoint(garStartSke, Config::getInstance()->clothSegOutPath+"garTop.cg");
	humSkeleton->dumpPoint(humStartSke, Config::getInstance()->clothSegOutPath+"humTop.cg");
	Vec3d humanSkeletonTrans = garSkeleton->nodeAt(garStartSke)->point - humSkeleton->nodeAt(humStartSke)->point;
	/* Translate Human skeleton */
	for(size_t s = 0; s < humSkeleton->nodeCount(); s++){
		humSkeleton->nodeAt(s)->point += humanSkeletonTrans;
	}
// 	const Skeleton::NodeList& nodes = humSkeleton->getNodeList();
// 	for(size_t i = 0; i < nodes.size(); i++){
// 		nodes[i]->point += humanSkeletonTrans;
// 	}
	humSkeleton->dump(Config::getInstance()->clothSegOutPath+"haha.cg");

	garSkeTrans.push_back(std::make_pair(garStartSke, Vec3d(0,0,0)));

	double garLen = 0;
	double humLen = 0;
	size_t humCurIndex = 0;
	double cachedLen = 0;
	Vec3d lastTrans;
	std::pair<Vec3d, double> lastRotate;
	for(size_t i = 1; i < garOrderedSkes.size(); i++){
		size_t curSke = garOrderedSkes[i];
		garLen += garSkeleton->nodeEuclideanDis(garOrderedSkes[i-1], curSke);		
		while(humLen < garLen){
			humCurIndex += 1;
			if(humCurIndex >= humOrderSkes.size())
				break;
			cachedLen = humSkeleton->nodeEuclideanDis(humOrderSkes[humCurIndex-1],
				humOrderSkes[humCurIndex]);
			humLen += cachedLen;
		}
		if(humCurIndex >= humOrderSkes.size()) // 假设袖子比手臂短
			break;
		Vec3d hp1 = humSkeleton->nodeAt(humOrderSkes[humCurIndex-1])->point;
		Vec3d hp2 = humSkeleton->nodeAt(humOrderSkes[humCurIndex])->point;
		Vec3d transVec = hp2 + (hp1-hp2) * (humLen-garLen)/cachedLen;
		lastTrans = transVec - garSkeleton->nodeAt(curSke)->point;
		garSkeTrans.push_back(std::make_pair(curSke, lastTrans));

		/* 计算骨骼旋转量 */
		Vec3d humanVec = hp2-hp1;
		const Vec3d& garCurPoint = garSkeleton->nodeAt(curSke)->point;
		Vec3d garVec = garCurPoint - garSkeleton->nodeAt(garOrderedSkes[i-1])->point;
		if(i + 1 < garOrderedSkes.size()){
			garVec += (garSkeleton->nodeAt(garOrderedSkes[i+1])->point - garCurPoint);
			garVec *= 0.5;
		}
		Vec3d axis = humanVec % garVec;
		double angle = -acos((humanVec|garVec)/(humanVec.length()*garVec.length()));
		lastRotate = std::make_pair(axis, angle);
		garSkeRotate.push_back(std::make_pair(curSke, lastRotate));
	}

	/* 处理比手臂长的袖子
	 * TODO
	 */


	/* Translate Human skeleton back */	
// 	for(size_t i = 0; i < nodes.size(); i++){
// 		nodes[i]->point -= humanSkeletonTrans;
// 	}
}

void SkeletonFitter::computeTranslation( VertexAlter_ ret, Region_ region, std::vector< std::pair<size_t, Vec3d> >& garSkeTrans )
{	
	Skeleton_ skeleton = this->getSkeleton(region);
	WatertightMesh_ mesh = region->getMesh();
	size_t skeCount = garSkeTrans.size();
	std::vector< size_t > involvedSkeNode;
	involvedSkeNode.reserve(3);
	for(size_t i = 0; i < skeCount; i++){
		involvedSkeNode.clear();
		involvedSkeNode.push_back(i);
		if((int)i - 1 >= 0 ){
			involvedSkeNode.push_back(i-1);
		}
		if(i + 1 < skeCount){
			involvedSkeNode.push_back(i+1);
		}
		SkeletonNode_ skeNode = skeleton->nodeAt(garSkeTrans[i].first);
		std::vector<size_t>& corVertices = skeNode->correspondanceIndices;
		size_t corCount = corVertices.size();
		for(auto it = corVertices.begin(); it !=  corVertices.end(); it++){
			size_t v = *it;
			Vec3d& vPoint = mesh->point(Mesh::VertexHandle(v));
			double sumDis = 0;
			std::vector<double> involvedDis;
			involvedDis.reserve(involvedSkeNode.size());
			for(auto ske_it = involvedSkeNode.begin();
				ske_it != involvedSkeNode.end(); ske_it++){
					Vec3d& skeP = skeleton->nodeAt(garSkeTrans[*ske_it].first)->point;
					double dis = (vPoint-skeP).length();
					involvedDis.push_back(dis);
					sumDis += dis;
			}
			Vec3d vDelta = Vec3d(0,0,0);
			for(size_t j = 0; j < involvedDis.size(); j++){
				vDelta += involvedDis[j]/sumDis * garSkeTrans[involvedSkeNode[j]].second;
			}
			ret->add(v, vDelta);
		}
	}
}

void SkeletonFitter::computeRotation( VertexAlter_ ret, Region_ region, 
										   std::vector< std::pair<size_t, std::pair<Vec3d, double> > >& garSkeRotates )
{
	size_t count = garSkeRotates.size();
	Skeleton_ skeleton = this->getSkeleton(region);
	WatertightMesh_ mesh = region->getMesh();
	for(size_t i = 0; i < count; i++){
		size_t skeNode = garSkeRotates[i].first;
		const std::pair<Vec3d, double>& rotation = garSkeRotates[i].second;
		const std::vector<size_t>& corrs = skeleton->nodeAt(skeNode)->correspondanceIndices;
		rotateCircle(ret, mesh, corrs, skeleton->nodeAt(skeNode)->point, rotation.first, rotation.second);
	}
}

VertexAlter_ SkeletonFitter::fit( Region_ humanRegion )
{
	VertexAlter_ ret = smartNew(VertexAlter);
	/* size_t：骨骼节点，Vec3d：骨骼节点位移向量 */
	std::vector< std::pair<size_t, Vec3d> > garSkeTrans;
	/* size_t：骨骼节点，Vec3d：骨骼节点旋转轴，double：旋转角 */
	std::vector< std::pair<size_t, std::pair<Vec3d, double> > > garSkeRotate;
	fitSkeleton(garSkeTrans, garSkeRotate, humanRegion);
	dumpDeformSkeleton(garSkeTrans);
	computeTranslation(ret, mGarmentRegion, garSkeTrans);
	computeRotation(ret, mGarmentRegion, garSkeRotate);
	return ret;
}

void SkeletonFitter::dumpDeformSkeleton(std::vector< std::pair<size_t, Vec3d> >& garSkeTrans)
{
#ifdef _DEBUG_
	Skeleton_ skeleton = this->getSkeleton(mGarmentRegion);
	for(size_t i = 0; i < garSkeTrans.size(); i++){
		skeleton->nodeAt(garSkeTrans[i].first)->point += garSkeTrans[i].second;
	}
	std::string name = mGarmentRegion->getMesh()->getName() + "_" + mGarmentRegion->getName() + "_deformed";
	skeleton->dump(Config::getInstance()->clothSegOutPath+name+".cg");
	for(size_t i = 0; i < garSkeTrans.size(); i++){
		skeleton->nodeAt(garSkeTrans[i].first)->point -= garSkeTrans[i].second;
	}
#endif
}
