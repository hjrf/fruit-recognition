#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

/** @函数main */
int main(int argc, char** argv)
{
	/// 声明变量
	Mat src, dst;

	Mat kernel;
	Point anchor;
	double delta;
	int ddepth;
	int kernel_size;
	char* window_name = "filter2D Demo";

	int c;

	/// 载入图像
	src = imread("hjr.jpg",1);

	if (!src.data)
	{
		return -1;
	}

	/// 创建窗口
	namedWindow(window_name, CV_WINDOW_AUTOSIZE);

	/// 初始化滤波器参数
	anchor = Point(-1, -1);
	delta = 0;
	ddepth = -1;

	/// 循环 - 每隔0.5秒，用一个不同的核来对图像进行滤波
	int ind = 0;
	while (true)
	{
		c = waitKey(500);
		/// 按'ESC'可退出程序
		if ((char)c == 27)
		{
			break;
		}

		/// 更新归一化块滤波器的核大小
		kernel_size = 3 + 2 * (ind % 5);
		kernel = Mat::ones(kernel_size, kernel_size, CV_32F) / (float)(kernel_size*kernel_size);

		/// 使用滤波器
		filter2D(src, dst, ddepth, kernel, anchor, delta, BORDER_DEFAULT);
		imshow(window_name, dst);
		ind++;
	}

	return 0;
}