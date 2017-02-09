#include "LevelSet.h"
size_t LevelNode::getNearestVertex( Mesh_ mesh )
{
	if(factor < 0.5){
		return start_vertex;
	}
	else{
		return getToVertexIndex(mesh);
	}
}

Vec3d LevelNode::exactlyPoint( Mesh_ mesh )
{
	Mesh::VertexHandle start_handle = Mesh::VertexHandle(start_vertex);
	Mesh::VertexHandle end_handle = Mesh::VertexHandle(getToVertexIndex(mesh));
	Vec3d& startPoint = mesh->point(start_handle);
	Vec3d ret =  startPoint + factor * (mesh->point(end_handle)- startPoint);
	return ret;
}

size_t LevelNode::getToVertexIndex( Mesh_ mesh )
{
	OpenMesh::EdgeHandle edgeHandle = OpenMesh::EdgeHandle(edge);
	if(mesh->is_boundary(edgeHandle)){
		PRINTLN("Error! In LevelNode. Watertight mesh should not has boundary edge.");
		return start_vertex;
	}
	OpenMesh::HalfedgeHandle heHandle = mesh->halfedge_handle(edgeHandle, 0);
	size_t ret = mesh->to_vertex_handle(heHandle).idx();
	if(ret == start_vertex)
		ret = mesh->from_vertex_handle(heHandle).idx();
	return ret;
}

LevelNode::LevelNode()
{

}

LevelNode::~LevelNode()
{

}


void LevelCircle::addNode( LevelNode_ node )
{
	levelNodes.push_back(node);
}

bool LevelCircle::isInThisCircle( LevelNode_ newNode, Mesh_ mesh )
{
	Mesh::EdgeHandle newEdge = Mesh::EdgeHandle(newNode->edge);
	for(size_t i = 0; i < levelNodes.size(); i++){
		if(isNeighbor(newEdge, Mesh::EdgeHandle(levelNodes[i]->edge), mesh)){
			return true;
		}
	}
	return false;
}

void LevelCircle::computeCenter( Mesh_ mesh )
{
	double minX = std::numeric_limits<double>::max();
	double maxX = std::numeric_limits<double>::min();
	for(size_t i = 0; i < levelNodes.size(); i++){
		Vec3d exactPoint = levelNodes[i]->exactlyPoint(mesh);
		double epX = exactPoint.values_[0];
		if(epX > maxX){
			maxX = epX;
		}
		if(epX < minX){
			minX = epX;
		}
	}
	mCenterX = 0.5 * (maxX + minX);
	isCenterXValid = true;
}

bool LevelCircle::isNeighbor( Mesh::EdgeHandle a, Mesh::EdgeHandle b, Mesh_ mesh )
{
	std::pair<size_t, size_t> avs = mesh->getEndVertices(a);
	std::pair<size_t, size_t> bvs = mesh->getEndVertices(b);
	if(avs.first == bvs.first || avs.first == bvs.second
		|| avs.second == bvs.first || avs.second == bvs.second)
		return true;
	return false;
}

double LevelCircle::getCenterX( Mesh_ mesh )
{
	if(isCenterXValid == false){
		computeCenter(mesh);
	}
	return mCenterX;
}

LevelCircle::LevelCircle()
{
	isCenterXValid = false;
}

Vec3d LevelCircle::getMeanPoint( Mesh_ mesh )
{
	Vec3d sum = Vec3d(0,0,0);
	for(size_t i = 0; i < levelNodes.size(); i++){
		Vec3d exactPoint = levelNodes[i]->exactlyPoint(mesh);
		sum += exactPoint;
	}
	return sum / levelNodes.size();
}

LevelSet::CircleLinkedList::CircleLinkedList()
{
	mHead = nullptr;
	mCur = nullptr;
	mCount = 0;
}

LevelSet::CircleLinkedList::CircleLinkedList( CircleLinkedList& other )
{
	mCount = other.mCount;
	mHead = nullptr;
	mCur = nullptr;
	int c = mCount;
	while(c--){
		add(other.cur());
		other.next();
	}
}

LevelSet::CircleLinkedList::~CircleLinkedList()
{
	while(isEmpty() == false){
		removeCurrent();
	}
}

LevelNode_ LevelSet::CircleLinkedList::cur()
{
	if(isEmpty()){
		return nullptr;
	}
	LevelNode_ ret = mCur->mNode;				
	return ret;
}

void LevelSet::CircleLinkedList::next()
{
	mCur = mCur->next;
	if(mCur == nullptr){
		mCur = mHead;				
	}
}

bool LevelSet::CircleLinkedList::isEmpty()
{
	return mHead == nullptr;
}

void LevelSet::CircleLinkedList::removeCurrent()
{
	if(isEmpty())
		return ;

	if(mCur->next == nullptr){
		mCur->mNode = mHead->mNode;
		mCur->next = nullptr;
		Node* tmp = mHead;
		mHead = mHead->next;
		delete tmp;
		mCur = mHead;
	}
	else{
		Node* n = mCur->next;
		mCur->mNode = n->mNode;
		mCur->next = n->next;
		delete n;
	}
	mCount -= 1;
}

void LevelSet::CircleLinkedList::add( LevelNode_ node )
{
	Node* n = new Node;
	n->mNode = node;
	n->next = mHead;
	mHead = n;
	if(mCur == nullptr){
		mCur = mHead;
	}
	mCount += 1;
}

size_t LevelSet::CircleLinkedList::getCount()
{
	return mCount;
}

LevelSet::LevelSet()
{
	mNodeCount = 0;
}

LevelSet::LevelSet( WatertightMesh_ mesh )
{
	mMesh = mesh;
}

LevelSet::~LevelSet()
{

}

void LevelSet::addNode( LevelNode_ node )
{
	mRawNodes.add(node);
}

void LevelSet::init()
{		
	classify();
}

size_t LevelSet::getCount()
{
	return mCircles.size();
}

LevelCircle_ LevelSet::getCircle( size_t categoryIndex )
{
	if(categoryIndex >=0 && categoryIndex < mCircles.size())
		return mCircles.at(categoryIndex);
	else{
		PRINTLN("ERROR! In LevelSet. Index out of boundary.");
		return nullptr;
	}
}

void LevelSet::dumpRaw( Mesh_ mesh, int i )
{
#ifdef _DEBUG_
	std::string path = "../data/levelset/";
	char back[20];
	sprintf(back, "raw%d.cg", i);
	std::ofstream out = std::ofstream(path + mesh->getName() + back);
	int edgeCount = mRawNodes.getCount();		
	out << "# D:3 NV:" << mesh->n_vertices() << " NE:" << edgeCount << "\n";
	for(Mesh::VertexIter vi = mesh->vertices_begin(); vi != mesh->vertices_end();
		vi++){
			Vec3d p = mesh->point(*vi);
			out << "v " << p.values_[0] << " " << p.values_[1] << " " << p.values_[2] << "\n";
	}
	while(edgeCount--){
		LevelNode_ ln = mRawNodes.cur();
		mRawNodes.next();
		auto vpair = mesh->getEndVertices(Mesh::EdgeHandle(ln->edge));
		out << "e " << vpair.first+1 << " " << vpair.second+1 << "\n";
	}									
	out.close();
#endif
}

void LevelSet::dump( int i )
{
#ifdef _DEBUG_
	std::string path = "../data/levelset/";
	char back[8];
	sprintf(back, "%d.cg", i);
	std::ofstream out = std::ofstream(path + mMesh->getName() + back);
	int edgeCount = 0;
	for(size_t i = 0; i < mCircles.size(); i++){
		edgeCount += mCircles[i]->levelNodes.size();
	}
	out << "# D:3 NV:" << mMesh->n_vertices()+mCircles.size() << " NE:" << edgeCount << "\n";
	for(Mesh::VertexIter vi = mMesh->vertices_begin(); vi != mMesh->vertices_end();
		vi++){
			Vec3d p = mMesh->point(*vi);
			out << "v " << p.values_[0] << " " << p.values_[1] << " " << p.values_[2] << "\n";
	}
	for(size_t i = 0; i < mCircles.size(); i++){
		LevelCircle_ lc = mCircles[i];
		SkeletonNode_ skn = mMesh->getSkeleton()->nodeAt(lc->mSkeNode);
		out << "v " << skn->point.values_[0] << " " << skn->point.values_[1] << " "
			<< skn->point.values_[2] << "\n";
	}
	for(size_t i = 0; i < mCircles.size(); i++){
		LevelCircle_ lc = mCircles[i];
		for(size_t j = 0; j < lc->levelNodes.size(); j++){
			LevelNode_ ln = lc->levelNodes[j];
			size_t v = ln->getNearestVertex(mMesh);
			out << "e " << v+1 << " " << mMesh->n_vertices()+i+1 << "\n";
//			auto vpair = mMesh->getEndVertices(Mesh::EdgeHandle(ln->edge));

//			out << "e " << vpair.first+1 << " " << vpair.second+1 << "\n";
		}
	}
	out.close();
#endif
}

void LevelSet::classify()
{
	if(mRawNodes.isEmpty())
		return;
	LevelCircle_ levelCircle = smartNew(LevelCircle);
	levelCircle->addNode(mRawNodes.cur());		
	mRawNodes.removeCurrent();
	mCircles.push_back(levelCircle);		
	int notInCircleCount = 0;
	while(mRawNodes.isEmpty() == false){								
		LevelNode_ n = mRawNodes.cur();
		if(levelCircle->isInThisCircle(n, mMesh)){
			levelCircle->addNode(n);
			mRawNodes.removeCurrent();
			notInCircleCount = 0;
		}
		else{
			notInCircleCount += 1;
			mRawNodes.next();
		}
		if(mRawNodes.isEmpty() == false && notInCircleCount >= mRawNodes.getCount()){
			levelCircle = smartNew(LevelCircle);
			levelCircle->addNode(mRawNodes.cur());		
			mRawNodes.removeCurrent();
			mCircles.push_back(levelCircle);
			notInCircleCount= 0;
		}
	}
}

void LevelSet::setMesh( WatertightMesh_ mesh )
{
	mMesh = mesh;
}

std::vector<LevelCircle_>& LevelSet::getCircles()
{
	return mCircles;
}
