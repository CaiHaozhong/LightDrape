#include <OpenMesh/Core/IO/MeshIO.hh>
#include "MeshSegmenter.h"
#include "GeodesicResolver.h"
#include "GeodesicResolverCached.h"
#include "LevelSetCacher.h"
#include "MeshSegmentListener.h"
#include <time.h>
#include "Mesher.h"
#include "RegionSkeletonizer.h"

MeshSegmenter::MeshSegmenter(void)
{
}

MeshSegmenter::MeshSegmenter( WatertightMesh_ mesh )
{
	init(mesh);
}


MeshSegmenter::~MeshSegmenter(void)
{
}

void MeshSegmenter::init( WatertightMesh_ mesh )
{
	mMesh = mesh;	
	PRINTLN("Begin compute Geodesic...");
	mGeodesicResolver = smartNew(GeodesicResolverCached);
	mGeodesicResolver->resolveGeo(mMesh);
	mGeodesicResolver->normalize(mMesh->getVertexPropertyGeoDis(), mMesh->n_vertices()); // �����ֵ��һ��
	mStart = clock();
	decideGranularity();
	PRINTLN("End compute Geodesic...");
	PRINTLN("Begin computeLevelSet...");

	computeLevelSet(true);
	PRINTLN("End computeLevelSet...");
}

void MeshSegmenter::segment()
{
	mSegment = createSegment();
	mMesh->setSegment(mSegment);
	onBeginSegmentHook();
	std::vector<bool> isNoise;		
	PRINTLN("Begin filterNoise...");
// 	for(size_t i = 0; i < mLevelSets.size(); i++){
// 		std::cout << i << ":" << mLevelSets[i]->getCount() << " ";
// 	}
// 	std::cout << "\n";
// 	for(size_t i = 0; i < mLevelSets.size(); i++){
// 		std::cout << mLevelSets[i]->getCount() << " ";
// 	}
// 	std::cout << "\n";
	filterNoise(isNoise);
// 	for(size_t i = 0; i < isNoise.size(); i++){
// 		std::cout << isNoise[i] << " ";
// 	}
// 	std::cout << "\n";
// 	for(size_t i = 0; i < isNoise.size(); i++){
// 		std::cout << i << ":" << isNoise[i] << " ";
// 	}
// 	std::cout << "\n";
	PRINTLN("End filterNoise...");
	clock_t start,end;
	start = clock();
	coarseSegment(isNoise);	
	end = clock();
	std::cout << "Time OF coarseSegment: " << end - start << "\n";
	start = clock();
	refineSegment();
	end = clock();
	std::cout << "Time OF refineSegment: " << end - start << "\n";
	std::cout << "Time OF Segment: " << end - mStart << "\n";
	onFinishSegmentHook();
}

WatertightMesh_ MeshSegmenter::getMesh() const
{
	return mMesh;
}

size_t MeshSegmenter::getLevelSetCount() const
{
	return mLevelSets.size();
}

LevelSet_ MeshSegmenter::getLevelSet( size_t i )
{
	if(i < mLevelSets.size())
		return mLevelSets[i];
	return nullptr;
}

double MeshSegmenter::getGranularity() const
{
	return mGranularity;
}


void MeshSegmenter::decideGranularity()
{
	//mGranularity = mMesh->getAverageEdgeLength();//1.7
	//mGranularity = 0.02;
	double len = mGeodesicResolver->getMaxGeoDis() - mGeodesicResolver->getMinGeoDis();
	mGranularity = (mMesh->getAverageEdgeLength()*0.8) / len;
}

void MeshSegmenter::computeLevelSet( bool useCache /*= false*/ )
{
	clock_t start, end;
	start = clock();
	LevelSetCacher lsc(mMesh);
	if(useCache && lsc.hasLevelSetCached()){
		PRINTLN("use cached levelset.");
		lsc.read(mLevelSets);
		return ;
	}
	std::vector<Mesh::EdgeHandle> meshEdges;
	for(Mesh::EdgeIter eit = mMesh->edges_begin(); 
		eit != mMesh->edges_end();eit++){
			meshEdges.push_back(*eit);
	}
	std::sort(meshEdges.begin(), meshEdges.end(),[this](Mesh::EdgeHandle& a,
		Mesh::EdgeHandle& b){				
			double minDisA = getMinDisFromEdge(a);
			double minDisB = getMinDisFromEdge(b);
			return minDisA < minDisB;
	});
	double curLevel = mGranularity;
	int stub = -1;//������һ��LevelSetʱ��һ�����������ıߣ�ʹ��curLevelλ�ڱ���
	size_t cursor = 0;
	mLevelSets.clear();
	mLevelSets.reserve(32);//TO DO
	mLevelSets.push_back(std::make_shared<LevelSet>(mMesh));	
	LevelSet_ levelSet = mLevelSets[mLevelSets.size()-1];
	levelSet->setHeight(curLevel);
	while(cursor < meshEdges.size()){	
		Mesh::EdgeHandle& e = meshEdges[cursor];
		std::pair<size_t, size_t> vs = mMesh->getEndVertices(e);
		std::pair<size_t, double> verDisPair01 
			= std::make_pair(vs.first, mMesh->getGeodesicDis(vs.first));
		std::pair<size_t, double> verDisPair02
			= std::make_pair(vs.second, mMesh->getGeodesicDis(vs.second));
		if(verDisPair01.second > verDisPair02.second){
			std::swap(verDisPair01, verDisPair02);
		}
		if(verDisPair01.second <= curLevel && verDisPair02.second >= curLevel){
			if(stub == -1){
				stub = cursor;
			}
			LevelNode_ node = smartNew(LevelNode);
			node->edge = e.idx();
			node->start_vertex = verDisPair01.first;
			/* ���������Ҷ���ȷ��������������ϴ�����򵥲�ֵ */
			node->factor = (curLevel-verDisPair01.second)
				/(verDisPair02.second-verDisPair01.second);	
			levelSet->addNode(node);
			++cursor;
		}
		else if(verDisPair01.second > curLevel){
			curLevel += mGranularity;
			mLevelSets.push_back(std::make_shared<LevelSet>(mMesh));
			levelSet = mLevelSets[mLevelSets.size()-1];
			levelSet->setHeight(curLevel);
			if(stub > -1){
				cursor = (size_t)stub;
				stub = -1;
			}
		}
		else{
			++cursor;
		}			
	}
	end = clock();
	std::cout << "Time OF compute levelset: " << end - start << "\n";
// 	for(size_t i = 0; i < mLevelSets.size(); i++){
// 		mLevelSets[i]->dumpRaw(i);
// 	}
	start = clock();
	for(size_t i = 0; i < mLevelSets.size(); i++){
//		char msg[50];
/*		sprintf(msg, "init levelset %d", i);*/
//		PRINTLN(msg);
		mLevelSets[i]->init();
// 		if(i % 2 == 0){
// 			mLevelSets[i]->dumpExactlyPoints(i);
// 		}
		//			mLevelSets[i]->dumpRaw(mMesh, i);
// 		if(mLevelSets[i]->getCount() == 5){
// 			for(size_t c = 0; c < mLevelSets[i]->getCount(); c++){
// 				getCircleSkeletonNode(mLevelSets[i]->getCircle(c));
// 			}
// 			mLevelSets[i]->dump(i);
// 		}
// 		 			for(size_t c = 0; c < mLevelSets[i]->getCount(); c++){
// 		 				getCircleSkeletonNode(mLevelSets[i]->getCircle(c));
// 		 			}
// 		 			mLevelSets[i]->dump(i);
	}
	end = clock();
	std::cout << "Time OF fenlei levelset: " << end - start << "\n";
	char c[20];
	sprintf(c, "%d", mLevelSets.size());
	PRINTLN(std::string("LevelSet Count ") + c);

	if(useCache){
		lsc.cache(mLevelSets);
		PRINTLN("cached levelset.");
	}
}

void MeshSegmenter::refineSegment()
{
	/* TO DO
	*/
}

void MeshSegmenter::coarseSegment( std::vector<bool>& isNoise )
{
	size_t len = mLevelSets.size();
	size_t seq = 1;
	size_t i = 0;
// 	while(i < len && isNoise[i]){
// 		++i;
// 	}
	size_t last = 0;
	if(i < len)
		last = mLevelSets[i]->getCount();
	for( ; i < len; ){
		if(! isNoise[i]){
			size_t cur = mLevelSets[i]->getCount();
			if(cur != last){
				++seq;
				last = cur;
			}
		}

		onDifferentLevelSet(seq, mLevelSets[i]);
		++i;
// 		while(i < len && isNoise[i]){
// 			++i;
// 		}
	}
	onFinishCoarseSegment();
}

void MeshSegmenter::onFinishCoarseSegment()
{
// 	std::vector< std::pair<int, Region_> >& regions = mSegment->getRegionsRaw();
// 	for(std::vector< std::pair<int, Region_> >::iterator it = regions.begin();
// 		it != regions.end(); it++){
// 			it->second->expand();
// 	}
	/* MeshSegmentListener */
// 	for(auto it = mListeners.begin(); it != mListeners.end(); it++){
// 		(*it)->onEndCoarseSegment(mSegment);
// 	}

}

double MeshSegmenter::getMinDisFromEdge( Mesh::EdgeHandle edge )
{
	std::pair<size_t, size_t> vs = mMesh->getEndVertices(edge);
	double minDis = std::min(mMesh->getGeodesicDis(vs.first),
		mMesh->getGeodesicDis(vs.second));
	return minDis;
}

void MeshSegmenter::filterNoise( std::vector<bool>& isNoise )
{
	isNoise.resize(mLevelSets.size(), false);
	size_t len = mLevelSets.size();
	int threshold = 13; // ��������С�ڵ���threshold�Ķ�������
	int val = mLevelSets[0]->getCount();
	int accu = 1;
	int start = 0;
	for(size_t i = 1; i < len; i++){
		int cur = mLevelSets[i]->getCount();
		if(cur == val){
			++accu;
		}
		else{				
			if(accu <= threshold){
				while(accu--){
					isNoise[start++] = true;
				}
			}
			start = i;
			val = cur;
			accu = 1;
		}			
	}
	if(accu <= threshold){
		while(accu--){
			isNoise[start++] = true;
		}
	}
}

void MeshSegmenter::addToRegion( Region_ region, LevelCircle_ levelCircle )
{
	region->addCircle(levelCircle);
	std::vector<LevelNode_>& levelNodes = levelCircle->levelNodes;
	for(size_t i = 0; i < levelNodes.size(); i++){
		LevelNode_ n = levelNodes[i];
		size_t v = n->getNearestVertex(mMesh);
		size_t skenode = mMesh->getCorrSkeletonNode(v);
		region->addSkeletonNode(skenode);
	}
	if(region->hasStartSetted() == false){
		region->setPossibleStart(*(region->getSkeNodes().begin()));
	}
}

size_t MeshSegmenter::getCircleSkeletonNode( LevelCircle_ levelCircle )
{
	std::map<size_t, size_t> nodeCountMap;
	for(size_t i = 0; i < levelCircle->levelNodes.size(); i++){
		LevelNode_ n = levelCircle->levelNodes[i];
		size_t v = n->getNearestVertex(mMesh);
		size_t skeNode = mMesh->getCorrSkeletonNode(v);
		if(nodeCountMap.find(skeNode) == nodeCountMap.end()){
			nodeCountMap[skeNode] = 1;
		}
		else{
			nodeCountMap[skeNode] += 1;
		}
	}
	int maxCount = -1;
	size_t ret = -1;
	for(std::map<size_t, size_t>::iterator it = nodeCountMap.begin();
		it != nodeCountMap.end(); it++){
			size_t sec = it->second;
			if((int)sec > maxCount){
				maxCount = it->second;
				ret = it->first;
			}
	}
	if(ret == -1){
		PRINTLN("ERROR! In getCircleSkeletonNode. ret == -1");
	}
	levelCircle->mSkeNode = ret;
	return ret;
}

void MeshSegmenter::onFinishSegmentHook(){}

void MeshSegmenter::onBeginSegmentHook(){}

size_t MeshSegmenter::getLevelSetIndex( LevelSet_ ls )
{
	return (size_t)(ls->getHeight() / getGranularity() + 0.5) - 1; //+0.5��Ϊ����������
}

void MeshSegmenter::addSegmentListener( MeshSegmentListener_ listener )
{
	mListeners.push_back(listener);
}

void MeshSegmenter::addSegmentListener(const std::vector<MeshSegmentListener_>& listeners)
{	
	for(auto it = listeners.begin(); it != listeners.end(); it++){
		mListeners.push_back(*it);
	}
}

void MeshSegmenter::handleNoise( LevelSet_ ls, std::vector<Region_>& curRegions )
{
	std::vector<LevelCircle_> circles = ls->getCircles();
	for(size_t i = 0; i < circles.size(); i++){
		LevelCircle_ cir = circles[i];
		if(curRegions.size() == 1){
			addToRegion(curRegions[0], cir);
			continue;
		}
		std::vector<LevelNode_>& nodes = cir->levelNodes;
		bool isAdd = false;
		for(size_t i = 0; i < nodes.size(); i++){
			Mesh::EdgeHandle eh = Mesh::EdgeHandle(cir->levelNodes[i]->edge);
			std::pair<size_t, size_t> evs = mMesh->getEndVertices(eh);	
			for(Mesh::VertexVertexIter vv_it = mMesh->vv_begin(
				Mesh::VertexHandle(evs.first)); vv_it.is_valid(); vv_it++){
					for(auto r_it = curRegions.begin(); r_it != curRegions.end(); r_it++){
						std::set<size_t>& vers = (*r_it)->getVertices();
						if(vers.find(vv_it->idx()) != vers.end()){
							addToRegion(*r_it, cir);
							isAdd = true;
							break;
						}
					}
					if(isAdd)
						break;
			}
			if(isAdd){
				break;
			}
			for(Mesh::VertexVertexIter vv_it = mMesh->vv_begin(
				Mesh::VertexHandle(evs.second)); vv_it.is_valid(); vv_it++){
					for(auto r_it = curRegions.begin(); r_it != curRegions.end(); r_it++){
						std::set<size_t>& vers = (*r_it)->getVertices();
						if(vers.find(vv_it->idx()) != vers.end()){
							addToRegion(*r_it, cir);
							isAdd = true;
							break;
						}
					}
					if(isAdd)
						break;
			}
			if(isAdd){
				break;
			}
		}
		if(!isAdd){
			addToRegion(curRegions[0], cir);
		}
	}
}

void MeshSegmenter::dumpRegion(Region_ re, std::string path)
{
#ifdef _DEBUG_	
	if(re == nullptr)
		return;		
	Mesher mesher(re);
	Mesh_ mesh = mesher.getMesh();
	mesh->request_vertex_normals();
	mesh->request_face_normals();	
	mesh->requestAABB();
	mesh->setName(mMesh->getName()+"_"+re->getName());
	WatertightMesh_ watertightMesh = std::make_shared<WatertightMesh>(mesh);
	watertightMesh->setName(mMesh->getName()+"_"+re->getName()+"_watertight");
	RegionSkeletonizer regionSkeletonizer;
//	Skeleton_ skeleton = regionSkeletonizer.skeletonize(re);		
	bool wsuc = OpenMesh::IO::write_mesh(*mesh, path+mesh->getName()+".obj");
	wsuc = OpenMesh::IO::write_mesh(*watertightMesh, path+watertightMesh->getName()+".obj");
//	skeleton->dump(path + mesh->getName() + ".cg");	
#endif
}
