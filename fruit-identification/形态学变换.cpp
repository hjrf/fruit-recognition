#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

/// 全局变量
Mat src, dst;

int morph_elem = 0;
int morph_size = 0;
int morph_operator = 0;
int const max_operator = 4;
int const max_elem = 2;
int const max_kernel_size = 21;

char* window_name = "Morphology Transformations Demo";

/** 回调函数申明 */
void Morphology_Operations(int, void*);

/** @函数 main */
int main(int argc, char** argv)
{
	/// 装载图像
	src = imread("hjr.jpg",1);

	if (!src.data)
	{
		return -1;
	}

	/// 创建显示窗口
	namedWindow(window_name, CV_WINDOW_AUTOSIZE);

	/// 创建选择具体操作的 trackbar
	createTrackbar("Operator:\n 0: Opening - 1: Closing \n 2: Gradient - 3: Top Hat \n 4: Black Hat", window_name, &morph_operator, max_operator, Morphology_Operations);

	/// 创建选择内核形状的 trackbar
	createTrackbar("Element:\n 0: Rect - 1: Cross - 2: Ellipse", window_name,
		&morph_elem, max_elem,
		Morphology_Operations);

	/// 创建选择内核大小的 trackbar
	createTrackbar("Kernel size:\n 2n +1", window_name,
		&morph_size, max_kernel_size,
		Morphology_Operations);

	/// 启动使用默认值
	Morphology_Operations(0, 0);

	waitKey(0);
	return 0;
}

/**
* @函数 Morphology_Operations
*/
void Morphology_Operations(int, void*)
{
	// 由于 MORPH_X的取值范围是: 2,3,4,5 和 6
	int operation = morph_operator + 2;

	Mat element = getStructuringElement(morph_elem, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));

	/// 运行指定形态学操作
	morphologyEx(src, dst, operation, element);
	imshow(window_name, dst);
}