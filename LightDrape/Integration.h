#pragma once
#include "Common.h"
#include "Vec3d.h"
#include <tuple>
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
	 * dt��ʱ�䲽��
	 * ����ֵ��
	 * pair.first��dtʱ����λ��
	 * pair.second��dtʱ�����ٶ�
	 */
	virtual std::pair<Vec3d, Vec3d> integrate(const Vec3d& x_t, const Vec3d& v_t, const Vec3d& f_t, double dt) = 0;
};
S_PTR(Integration);

