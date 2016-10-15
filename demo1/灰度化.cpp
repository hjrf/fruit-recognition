#include "灰度化.h"

/// 应用直方图均衡化
Mat Huidu::Process(Mat inputImg)
{
	Mat huidu;
	cvtColor(inputImg, huidu, CV_BGR2GRAY);
	return huidu;
}