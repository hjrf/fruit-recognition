#include "直方图均衡化.h"

/// 应用直方图均衡化
Mat HistogramAvg::Process (Mat inputImg)
{
	Mat avg;
	equalizeHist(inputImg, avg);
	return avg;
}