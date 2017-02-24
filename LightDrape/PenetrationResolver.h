#pragma once
#include "Common.h"
#include "Vec3d.h"
#include <vector>
class Mesh;
S_PTR(Mesh);
class KNNSHelper;
S_PTR(KNNSHelper);
/* �Ƴ������б����������֮��Ĵ�͸
 * ���ı��������
 */
class PenetrationResolver
{
private:
	Mesh_ mRigidMesh;

	/* ������������ľ��� */
	double mEPS;

	/* Ŀ�궥������ӱ� */

	std::vector<std::vector<size_t> > mTargetAdjList;

	/* ���������������� */
	KNNSHelper_ mKNNSHelper;
public:
	PenetrationResolver(void);
	~PenetrationResolver(void);

	/* ���ø������� */
	void setRigidMesh(Mesh_ rigid);

	/* ����Ŀ�궥������ӹ�ϵ */
	void setAdjList(std::vector<std::vector<size_t> >& adjList);

	/* ���д�͸����
	 * ��������������
	 * �����ӹ�ϵ��setAdjList(std::vector<std::vector<size_t> >&)����
	 * penetration�����ؽ����͸֮ǰ���д�͸�Ķ���
	 * ����ֵ������д�͸���򷵻�false�����򷵻�true
	 */
	bool resolve(std::vector<Vec3d>& points, std::vector<size_t>& penetrationVers = std::vector<size_t>());

	/* ���÷�װ������Ƥ���ľ��룬 ��λΪcm��Ĭ��Ϊ0.5cm */
	void setAllowDis(double eps);
private:
	/* �����װ��ÿ�����񶥵��Ӧ�ľ���������������񶥵� 
	 * ���������nnsHumanVertex�У�Ԫ��Ϊ�������񶥵���±�
	 */ 
	void computeNNSHumanVertex(std::vector<int>& nnsHumanVertex, std::vector<Vec3d>& points );

	/* ����õ��������д�͸���·����񶥵㣬�����ڲ�������penetrationTest��
	 * penetrationTest �Ĵ�С�����·����������������Ԫ�س�ʼ��Ϊ0
	 * ����ֵ�����û�д�͸�Ķ��㣬�򷵻�false�����򷵻�true
	 */
	bool computePenetrationVertices(const std::vector<int>& nearestHumanVertex, std::vector<bool>& penetrationTest, std::vector<Vec3d>& points );

};
S_PTR(PenetrationResolver);

