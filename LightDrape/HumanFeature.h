#pragma once
#include <string>
/* ---------------- 人体特征 ---------------- */
class HumanFeature
{
public:
	/* 身高 */
	double height;

	/* 脖子长度 */
	double neckLength;

	/* 肩高 */
	double shouldHeight;

	/* 腿长 */
	double legLength;

	/* 肩宽：一半肩膀的宽度，不包括脖子 */
	double shoulderDis;

	/* 前胸宽 */
	double frontChestDis;

public:
	HumanFeature(void);
	~HumanFeature(void);

	/* 从文件中读取，该文件包含了makehuman建模标签中的measure中的测量数据
	 * 若人体模型由makehuman导出，则调用这个函数来确定人体的特征
	 */
	void fromMakeHumanMeasureFile(std::string fileName);
};

