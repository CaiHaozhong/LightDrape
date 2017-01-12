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
	static Config_ getInstance();
};