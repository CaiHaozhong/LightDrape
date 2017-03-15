#include "GeodesicResolverCached.h"


GeodesicResolverCached::GeodesicResolverCached(void)
{
}


GeodesicResolverCached::~GeodesicResolverCached(void)
{
}

DoubleProperty_ GeodesicResolverCached::resolveGeo( WatertightMesh_ mesh )
{
	if(hasGeoCached(mesh)){
		PRINTLN("Use cached geo.");
		DoubleProperty_ ret = readCached(mesh, getGeoFileName(mesh));
		mesh->setVertexPropertyGeoDis(ret);
		return ret;
	}
	else{
		//DoubleProperty_ ret = GeodesicResolver::resolveGeo(mesh);
		DoubleProperty_ ret = agd(mesh);
		//DoubleProperty_ ret = hei(mesh);
		cachedGeo(ret, mesh->n_vertices(), getGeoFileName(mesh));
		PRINTLN("Cached geo success.");
		return ret;
	}
}

std::string GeodesicResolverCached::getGeoFileName( WatertightMesh_ mesh )
{
	std::string name = mesh->getName();
	std::string path = "..\\data\\geo_cache\\";
	return path + name + ".geo";
}

bool GeodesicResolverCached::hasGeoCached( WatertightMesh_ mesh )
{
	std::string name = getGeoFileName(mesh);
	FILE* file = fopen(name.c_str(), "r");
	if(file != nullptr){
		fclose(file);
		return true;
	}
	else{
		return false;
	}
}

void GeodesicResolverCached::cachedGeo( DoubleProperty_ pro, size_t vertexCount, std::string file )
{
	std::ofstream out(file);
	out << vertexCount << "\n";
	for(size_t i = 0; i < vertexCount; i++){
		out << pro->get(i) << "\n";
	}
	out.close();
}

DoubleProperty_ GeodesicResolverCached::readCached( Mesh_ mesh, std::string file )
{
	DoubleProperty_ ret = smartNew(DoubleProperty);
	mesh->registerProperty(ret);
	std::ifstream in(file);
	int count;
	double dis;
	in >> count;
	mMaxDis = std::numeric_limits<double>::min();
	mMinDis = std::numeric_limits<double>::max();
	for(int i = 0; i < count; i++){
		in >> dis;
		ret->set(i, dis);
		if(dis > mMaxDis){
			mMaxDis = dis;
		}
		if(dis < mMinDis){
			mMinDis = dis;
		}
	}
	return ret;
}

DoubleProperty_ GeodesicResolverCached::agd( WatertightMesh_ mesh )
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

DoubleProperty_ GeodesicResolverCached::hei( WatertightMesh_ mesh )
{
	DoubleProperty_ ret = smartNew(DoubleProperty);
	mesh->registerProperty(ret);
	mesh->setVertexPropertyGeoDis(ret);
	size_t vCount = mesh->n_vertices();
	for(Mesh::VertexIter it = mesh->vertices_begin(); it != mesh->vertices_end(); it++){		
		size_t s = (*it).idx();
		ret->set(s, mesh->point(*it).values_[1]);
	}
	/*normalize(ret, vCount);*/
	return ret;
}
