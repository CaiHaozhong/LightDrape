#include "AGDResoler.h"


AGDResoler::AGDResoler(void)
{
}


AGDResoler::~AGDResoler(void)
{
}

DoubleProperty_ AGDResoler::resolveGeo( WatertightMesh_ mesh )
{
	DoubleProperty_ ret = smartNew(DoubleProperty);
	mesh->registerProperty(ret);
	mesh->setVertexPropertyGeoDis(ret);
	GeodesicAdapter geodesicAdapter(*mesh);
	size_t cur = 0;
	size_t vCount = mesh->n_vertices();
	double percent = 0;
	for(Mesh::VertexIter it = mesh->vertices_begin(); it != mesh->vertices_end(); it++){
		cur += 1;
		size_t s = (*it).idx();
		ret->set(s, geodesicAdapter.averageGeodesicDisFromSource(s));
		double curP = (double)cur / vCount * 100;
		std::cout << cur << " ";
		if(curP - percent >= 2){
			std::cout << "Percent: " << curP << "%\n";
			percent = curP;
		}		
	}
	/*normalize(ret, vCount);*/
	return ret;
}
