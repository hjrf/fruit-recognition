#include "canny边缘检测.h"
/// 全局变量
int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
Mat EdgeCanny::Process(Mat inputImg)
{
	Mat gard_canny;
	/// 使用 3x3内核降噪
	blur(inputImg, gard_canny, Size(3, 3));
	/// 运行Canny算子
	Canny(gard_canny, gard_canny, lowThreshold, lowThreshold*ratio, kernel_size);

	return gard_canny;
}
