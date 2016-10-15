#pragma once
#ifndef _图像处理_H_
#define _图像处理_H_

#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <fstream>

using namespace cv;
using namespace std;

class ImgProcess {
public:
	virtual Mat Process(Mat inputImg) = 0;

};


#endif // !<GONGCHANG>