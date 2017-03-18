#pragma once
#include <string>
/* ---------------- �������� ---------------- */
class HumanFeature
{
public:
	/* ��� */
	double height;

	/* ���ӳ��� */
	double neckLength;

	/* ��� */
	double shouldHeight;

	/* �ȳ� */
	double legLength;

	/* ���һ����Ŀ�ȣ����������� */
	double shoulderDis;

	/* ǰ�ؿ� */
	double frontChestDis;

	/* ���ȵ�ֱ�� */
	double upperLegWidth;

	/* �¸��߶� */
	double underbellyHeight;

	/* ����������ĸ߶� */
	double clothHeight;

	/* ��ͷ������ĸ߶� */
	double trousersHeight;
public:
	HumanFeature(void);
	~HumanFeature(void);

	/* ���ļ��ж�ȡ�����ļ�������makehuman��ģ��ǩ�е�measure�еĲ�������
	 * ������ģ����makehuman��������������������ȷ�����������
	 */
	void fromMakeHumanMeasureFile(std::string fileName);
};

