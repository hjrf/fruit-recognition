#include"Èñ»¯.h"

Mat RuiHua::Process(Mat inputImg)
{
	Mat ruiHua;
	Mat kernel(3,3,CV_32F,Scalar(-1));
	filter2D(inputImg, ruiHua, inputImg.depth(), kernel);
	return ruiHua;
}