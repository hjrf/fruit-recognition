#include "高斯滤波.h"

/// 应用直方图均衡化
Mat LvboGaoSi::Process(Mat inputImg)
{
	Size s = Size(3, 3);
	Mat lvbo_gaosi;
	GaussianBlur(inputImg, lvbo_gaosi, s, 0, 0);
	return lvbo_gaosi;
}