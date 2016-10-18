#pragma once
#ifndef _直方图均衡化_H_
#define _直方图均衡化_H_

#include "图像处理.h"

class HistogramAvg : public ImgProcess
{
	Mat Process(Mat inputImg);
};

#endif // !<GONGCHANG>