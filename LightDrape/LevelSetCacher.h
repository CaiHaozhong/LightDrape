#pragma once
#include "Common.h"
#include <string>
#include <vector>
#include <fstream>
class WatertightMesh;
class LevelSet;
S_PTR(WatertightMesh);
S_PTR(LevelSet);
class LevelSetCacher
{
private:
	std::string mPath;
	WatertightMesh_ mMesh;
public:

	LevelSetCacher(WatertightMesh_ mesh);

	~LevelSetCacher(void);
	
	void setMesh(WatertightMesh_ mesh);

	bool hasLevelSetCached();

	void read(std::vector<LevelSet_>& levelSets);

	void cache(std::vector<LevelSet_>& levelSets);

private:
	std::string getName();

	void cacheLevelSet(std::ofstream& out, LevelSet_ levelSet);

	LevelSet_ readLevelSet(std::ifstream& in);

};

