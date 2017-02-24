#pragma once
#include "integration.h"
/* ����Runge-Kutta���ַ� */
class RungeKuttaIntegration :
	public Integration
{
public:
	RungeKuttaIntegration(void);
	~RungeKuttaIntegration(void);

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
	std::pair<Vec3d, Vec3d> integrate2(const Vec3d& x_t, const Vec3d& v_t, const Vec3d& f_t, const double mass, double dt);


	std::pair<Vec3d, Vec3d> integrate(size_t i, const std::vector<Vec3d>& curPositions,
		const std::vector<Vec3d>& curVelocities, const std::vector<double>& pointMass, 
		const std::vector<ComponentForce_> forces, double dt);
};

