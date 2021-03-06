#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "Common.h"
class Config;
S_PTR(Config);
class Config{
private:
	static Config_ mInstance;
	Config();
	void init(char* file);
public:
	std::string humanInPath;	
	std::string clothInPath;
	std::vector<std::string> humanInFileNames;
	std::vector<std::string> clothInFileNames;
	std::string mtlFileName;
	std::string useMtlName;
	std::string clothOutPath;
	std::string humanMeshDiretion;
	std::string humanSegOutPath;
	std::string clothSegOutPath;
	std::string clothType;
	bool humanRefiner;
	bool humanSimpleRefiner;
	bool clothRefiner;
	bool clothSimpleRefiner;

	double humanLegCenterXep;

	static Config_ getInstance();

	/* for physical simulation */
	double gravity;
	double mass;
	double struct_spring_ks, struct_spring_kd;
	double damping_force;	
	double bend_spring_ks, bend_spring_kd;
	double step;
	double simulateLen;

	double qualitySpeedTradeoff;

	std::vector<int> resultFrames;

	double skeletonTime, holefillTime, deformTime;
};