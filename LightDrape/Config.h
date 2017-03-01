#pragma once
#include <string>
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
	std::string humanInFileName;
	std::string clothInFileName;
	std::string clothOutPath;
	std::string humanMeshDiretion;
	std::string humanSegOutPath;
	std::string clothSegOutPath;
	static Config_ getInstance();

	/* for physical simulation */
	double gravity;
	double mass;
	double struct_spring_ks, struct_spring_kd;
	double damping_force;	
	double bend_spring_ks, bend_spring_kd;
	double step;
	double simulateLen;
};