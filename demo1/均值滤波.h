#pragma once
#ifndef _均值滤波_H_
#define _均值滤波_H_

#include "图像处理.h"

class LvboAvg : public ImgProcess
{
	Mat Process(Mat inputImg);
};


#endif // !<GONGCHANG>