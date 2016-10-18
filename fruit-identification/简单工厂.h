#pragma once
#ifndef _简单工厂_H_
#define _简单工厂_H_

#include "图像处理.h"
#include "灰度化.h"
#include "二值化.h"
#include "canny边缘检测.h"
#include"sebel导数.h"
#include"均值滤波.h"
#include "直方图均衡化.h"
#include "锐化.h"

void drawText(Mat & inputImg, string text);

class SimpleFactory
{
public:
	static ImgProcess * Create(const std::string & model);
};


#endif // !<GONGCHANG>