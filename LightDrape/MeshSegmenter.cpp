#include "MeshSegmenter.h"
#include "GeodesicResolver.h"
#include "GeodesicResolverCached.h"
#include "LevelSetCacher.h"
#include "MeshSegmentListener.h"

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
	mGeodesicResolver->normalize(mMesh->getVertexPropertyGeoDis(), mMesh->n_vertices()); // 将测地值归一化
	decideGranularity();
	PRINTLN("End compute Geodesic...");
	PRINTLN("Begin computeLevelSet...");
	computeLevelSet();
	PRINTLN("End computeLevelSet...");
}

void MeshSegmenter::segment()
{
	mSegment = createSegment();
	mMesh->setSegment(mSegment);
	onBeginSegmentHook();
	std::vector<bool> isNoise;		
	PRINTLN("Begin filterNoise...");
	for(size_t i = 0; i < mLevelSets.size(); i++){
		std::cout << mLevelSets[i]->getCount() << " ";
	}
	std::cout << "\n";
	filterNoise(isNoise);
	for(size_t i = 0; i < isNoise.size(); i++){
		std::cout << isNoise[i] << " ";
	}
	std::cout << "\n";
	PRINTLN("End filterNoise...");
	coarseSegment(isNoise);	
	refineSegment();
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
	int stub = -1;//计算上一个LevelSet时第一条符合条件的边，使得curLevel位于边上
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
			/* 可以用余弦定理精确求出，但计算量较大，这里简单插值 */
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
	for(size_t i = 0; i < mLevelSets.size(); i++){
		mLevelSets[i]->dumpRaw(i);
	}
	for(size_t i = 0; i < mLevelSets.size(); i++){
		char msg[50];
		sprintf(msg, "init levelset %d", i);
		PRINTLN(msg);
		mLevelSets[i]->init();
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
	while(i < len && isNoise[i]){
		++i;
	}
	size_t last = 0;
	if(i < len)
		last = mLevelSets[i]->getCount();
	for( ; i < len; ){
		size_t cur = mLevelSets[i]->getCount();
		if(cur != last){
			++seq;
			last = cur;
		}
		onDifferentLevelSet(seq, mLevelSets[i]);
		++i;
		while(i < len && isNoise[i]){
			++i;
		}
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
	int threshold = 12; // 连续个数小于等于threshold的都算噪音
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
	std::vector<LevelNode_> levelNodes = levelCircle->levelNodes;
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
	return (size_t)(ls->getHeight() / getGranularity() + 0.5) - 1; //+0.5是为了四舍五入
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