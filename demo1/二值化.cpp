#include "二值化.h"

///对输入图像做二值化处理（最好是输入灰度图像）
Mat Value2::Process(Mat inputImg)
{
	Mat value2;
	value2 = inputImg.clone();
	int rows = value2.rows;
	int cols = value2.cols;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (value2.ptr<uchar>(i)[j] > 128)
			{
				value2.ptr<uchar>(i)[j] = 255;
			}
			else
			{
				value2.ptr<uchar>(i)[j] = 0;
			}
		}
	}
	return value2;
}
