#include "LevelSetCacher.h"
#include "WatertightMesh.h"
#include "LevelSet.h"
#include <fstream>


LevelSetCacher::LevelSetCacher( WatertightMesh_ mesh = nullptr)
{	
	mPath = "..\\data\\levelset\\";
	mMesh = mesh;
}


LevelSetCacher::~LevelSetCacher(void)
{
}

bool LevelSetCacher::hasLevelSetCached()
{
	if(mMesh == nullptr){
		return false;
	}
	std::string fileName = getName();
	FILE* f = fopen(fileName.c_str(), "r");
	if(f == nullptr){
		return false;
	}
	fclose(f);
	return true;
}

void LevelSetCacher::read( std::vector<LevelSet_>& levelSets )
{
	if(mMesh == nullptr){
		return;
	}
	std::string fileName = getName();
	std::ifstream in(fileName);
	size_t count;
	in >> count;
	while(count--){
		levelSets.push_back(readLevelSet(in));
	}
	in.close();

}

void LevelSetCacher::cache( std::vector<LevelSet_>& levelSets )
{
	if(mMesh == nullptr){
		return;
	}
	std::string fileName = getName();
	std::ofstream out(fileName);
	size_t count = levelSets.size();
	out << count << "\n";
	for(size_t i = 0; i < count; i++){
		cacheLevelSet(out, levelSets[i]);
	}
	out.close();
}

void LevelSetCacher::setMesh( WatertightMesh_ mesh )
{
	mMesh = mesh;
}

void LevelSetCacher::cacheLevelSet(std::ofstream& out, LevelSet_ levelSet )
{
	size_t count = levelSet->getCount();
	out << count << "\n";
	for(size_t i = 0; i < count; i++){
		LevelCircle_ lc = levelSet->getCircle(i);
		size_t nodeCount = lc->levelNodes.size();
		out << nodeCount << "\n";
		for(size_t j = 0; j < nodeCount; j++){
			LevelNode_ ln = lc->levelNodes[j];
			out << ln->edge << " " << ln->start_vertex << " " << ln->factor << "\n";
		}
	}
}

LevelSet_ LevelSetCacher::readLevelSet( std::ifstream& in )
{
	LevelSet_ ret = smartNew(LevelSet);
	ret->setMesh(mMesh);
	std::vector<LevelCircle_>& circlesInRet = ret->getCircles();	
	size_t circleCount;
	in >> circleCount;
	while(circleCount--){
		LevelCircle_ lc = smartNew(LevelCircle);
		size_t nodeCount;
		in >> nodeCount;
		while(nodeCount--){
			LevelNode_ ln = smartNew(LevelNode);
			in >> ln->edge >> ln->start_vertex >> ln->factor;
			lc->addNode(ln);
		}
		circlesInRet.push_back(lc);
	}
	return ret;
}

std::string LevelSetCacher::getName()
{
	if(mMesh == nullptr)
		return "";
	return mPath + mMesh->getName() + ".ls";
}

