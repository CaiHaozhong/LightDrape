#include "HumanFeature.h"
#include <fstream>

HumanFeature::HumanFeature(void)
{
}


HumanFeature::~HumanFeature(void)
{
}


void HumanFeature::fromMakeHumanMeasureFile( std::string fileName )
{
	std::ifstream in(fileName);
	std::string featureName[] = {"height", "neck_height", "nape_to_waist",
	"waist_to_hip", "shoulder_dist", "front_chest_dist", "lowerleg_height",
	"upperleg_height", "shoulder_height","upperleg_width", "underbelly_height",
	"cloth_height", "trousers_height"};
	double nape_to_waist, waist_to_hip, lowerleg_height, upperleg_height;
	double* value[] = {&height, &neckLength, &nape_to_waist, &waist_to_hip,
	&shoulderDis, &frontChestDis, &lowerleg_height, &upperleg_height, &shouldHeight,
	&upperLegWidth, &underbellyHeight, &clothHeight, &trousersHeight};
	std::string str;
	size_t featureCount = sizeof(featureName)/sizeof(featureName[0]);
	while(!in.eof()){
		in >> str;
		if(str == "#"){
			getline(in, str);
			continue;
		}
		for(size_t i = 0; i < featureCount; i++){
			if(featureName[i] == str){
				in >> *(value[i]);
				break;
			}
		}
	}
	legLength = lowerleg_height + upperleg_height;
}
