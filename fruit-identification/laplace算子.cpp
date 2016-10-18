#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

/** @函数 main */
int main(int argc, char** argv)
{
	Mat src, src_gray, dst;
	int kernel_size = 3;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	char* window_name = "Laplace Demo";

	int c;

	/// 装载图像
	src = imread("hjr.jpg",1);

	if (!src.data)
	{
		return -1;
	}

	/// 使用高斯滤波消除噪声
	GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);

	/// 转换为灰度图
	cvtColor(src, src_gray, CV_RGB2GRAY);

	/// 创建显示窗口
	namedWindow(window_name, CV_WINDOW_AUTOSIZE);

	/// 使用Laplace函数
	Mat abs_dst;

	Laplacian(src_gray, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(dst, abs_dst);

	/// 显示结果
	imshow(window_name, abs_dst);

	waitKey(0);

	return 0;
}