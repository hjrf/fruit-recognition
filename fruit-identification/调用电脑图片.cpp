#include "¼òµ¥¹¤³§.h"

Mat UseImg(string imgName,string modelName)
{
	Mat image,outputImg;
	image = imread(imgName, 1);	
	ImgProcess * IP = SimpleFactory::Create(modelName);
	outputImg = IP->Process(image);
	return outputImg;
}


