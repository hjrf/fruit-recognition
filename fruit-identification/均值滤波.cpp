#include "均值滤波.h"
Size s = Size(3, 3);
/// 应用直方图均衡化
Mat LvboAvg::Process (Mat inputImg)
{
	Mat lvbo_avg;
	blur(inputImg, lvbo_avg,s, Point(-1, -1));
	return lvbo_avg;
}