//#include "简单工厂.h"
//
//Mat UseImg(string imgName, string modelName);
//int UseCapture(string modelName);
//string WINDOW_NAME = "水果识别";
//string shuiguo [] = {"pingguo.jpg","xiangjiao.jpg","juzi.jpg","li.jpg"};
//
//string modelName[] = {"二值化","均值滤波","sebel导数","canny边缘检测","直方图均衡化"};
//int barValue = 0;
//int barValueMax = 4;
//void OnTrackbar(int,void*)
//{
//	//imshow(WINDOW_NAME, UseImg("pingguo.jpg", modelName[barValue]));
//	UseCapture(modelName[barValue]);
//}
//
//int main()
//{
//	namedWindow(WINDOW_NAME,1);
//	createTrackbar("图片：", WINDOW_NAME, &barValue,barValueMax, OnTrackbar);
//	OnTrackbar(barValue,0);
//	waitKey(0);
//	return 0;
//}
//
//
