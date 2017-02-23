#pragma once
#include "Common.h"
#include <vector>
class Mesh;
S_PTR(Mesh);
/* �Ƴ��·���������������֮��Ĵ�͸
 * ���ı���������
 * ��ı��·����񶥵��λ��
 */
class PenetrationResolver
{
protected:
	Mesh_ mGarment;
	Mesh_ mHuman;
	double mEPS;
public:
	PenetrationResolver(void);
	PenetrationResolver(Mesh_ garment);
	~PenetrationResolver(void);
	void setGarmentMesh(Mesh_ garment);
	void setHumanMesh(Mesh_ human);

	/* ���д�͸���� */
	bool resolve();

	/* ���÷�װ������Ƥ���ľ��룬 ��λΪcm��Ĭ��Ϊ0.5cm */
	void setGarHumDis(double eps);
protected:
	/* �����װ��ÿ�����񶥵��Ӧ�ľ���������������񶥵� 
	 * ���������nnsHumanVertex�У�Ԫ��Ϊ�������񶥵���±�
	 */ 
	void computeNNSHumanVertex(std::vector<int>& nnsHumanVertex );

	/* ����õ��������д�͸���·����񶥵㣬�����ڲ�������penetrationTest��
	 * penetrationTest �Ĵ�С�����·����������������Ԫ�س�ʼ��Ϊ0 */
	void computePenetrationVertices(const std::vector<int>& nearestHumanVertex, std::vector<bool>& penetrationTest );

	/* �������񶥵���ڽӱ� */
	void computeAdjList( Mesh_ mesh, std::vector< std::vector<size_t> >& adjList );
};
S_PTR(PenetrationResolver);

