#include "Config.h"

Config_ Config::mInstance = nullptr;

Config::Config()
{

}

void Config::init( char* file )
{
	std::string type;
	std::string ignore;
	std::ifstream configIn(file);
	while(!configIn.eof()){
		configIn >> type;
		if(type == "#"){
			getline(configIn, ignore);
			continue;
		}
		if(type == "human_in_path"){
			configIn >> humanInPath;
		}
		else if(type == "cloth_in_path"){
			configIn >> clothInPath;
		}
		else if(type == "human_in_file"){
			configIn >> humanInFileName;
		}
		else if(type == "cloth_in_file"){
			configIn >> clothInFileName;
		}
		else if(type == "cloth_out_path"){
			configIn >> clothOutPath;
		}
		else if(type == "human_mesh_diretion"){
			configIn >> humanMeshDiretion;
		}
		else if(type == "human_seg_out_path"){
			configIn >> humanSegOutPath;
		}
		else if(type == "cloth_seg_out_path"){
			configIn >> clothSegOutPath;
		}
	}
}

Config_ Config::getInstance()
{
	if(mInstance == nullptr){
		mInstance = std::shared_ptr<Config>(new Config());
		mInstance->init("D:\\Develop\\project\\LightDrape\\C++\\data\\config");
	}
	return mInstance;
}
