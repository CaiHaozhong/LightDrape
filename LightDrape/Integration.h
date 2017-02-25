#pragma once
#include "Common.h"
#include "Vec3d.h"
#include <tuple>
#include <vector>
class Mesh;
S_PTR(Mesh);
class ComponentForce;
S_PTR(ComponentForce);
/* ������ⷨ */
class Integration
{
public:
	Integration(void);
	~Integration(void);

	/* �������һ��ʱ�䲽�����λ�ú��ٶ� 
	 * ������
	 * x_t����ǰ��λ��
	 * v_t����ǰ���ٶ�
	 * f_t����ǰ�ĺ���
	 * mass���ʵ������
	 * dt��ʱ�䲽��
	 * ����ֵ��
	 * pair.first��dtʱ����λ��
	 * pair.second��dtʱ�����ٶ�
	 */
	virtual std::pair<Vec3d, Vec3d> integrate2(const Vec3d& x_t, const Vec3d& v_t, const Vec3d& f_t, const double mass, double dt) = 0;

	virtual void integrate(
		std::vector<Vec3d>& curPositions,
		std::vector<Vec3d>& curVelocities, 
		const std::vector<double>& pointMass, 
		const Mesh_ mesh,
		const std::vector<ComponentForce_>& forces, 
		double dt) = 0;
};
S_PTR(Integration);

