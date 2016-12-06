#include "KNNSHelper.h"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
using namespace std;
int main(){
	const int N = 10000;
	vector<OpenMesh::Vec3f> points;
	points.resize(N);
	srand(time(NULL));
	for (size_t i = 0; i < N; i++)
	{
		points[i] = OpenMesh::Vec3f(10 * (rand() % 1000) / float(1000),
									10 * (rand() % 1000) / float(1000),
									10 * (rand() % 1000) / float(1000));
	}
	KNNSHelper knnsHelper(points);	
	OpenMesh::Vec3f q(0.5f, 0.5f, 0.5f);	
	KNNSHelper::Result ret;
	bool r = knnsHelper.singleNeighborSearch(q, ret);	
	if(r){
		cout << ret.index << "\n";
		cout << points[ret.index].values_[0] << " " << points[ret.index].values_[1] << " " << points[ret.index].values_[2] << "\n";
		cout << ret.distance << "\n";
	}

	getchar();
}