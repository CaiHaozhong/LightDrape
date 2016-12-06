#include "KNNSHelper.h"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
using namespace std;
int main(){
	const int N = 100;
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
	vector<KNNSHelper::Result> ret;
	int K = 10;
	OpenMesh::Vec3f q(0.5, 0.5, 0.5);
	bool r = knnsHelper.kNeighborSearch(q, K, ret);
	for (size_t i = 0; i < ret.size(); i++){
		cout << "index: " << ret[i].index << "\t" << "distance: " << ret[i].distance << "\n";
	}
	getchar();
}