//#include "简单工厂.h"
//
//Mat image;
//ImgProcess * c_value2;
//ImgProcess * c_huidu;
//Mat huidu;
//Mat value2;
//string WINDOW_NAME = "水果识别";
//string shuiguo[] = { "pingguo.jpg","xiangjiao.jpg","juzi.jpg","li.jpg" };
//
//int barValue = 0;
//int barValueMax = 3;
//void OnTrackbar(int, void*)
//{
//	image = imread(shuiguo[barValue], 1);
//	c_value2 = SimpleFactory::Create("二值化");
//	c_huidu = SimpleFactory::Create("灰度化");
//	huidu = c_huidu->Process(image);
//	value2 = c_value2->Process(huidu);
//	imshow(WINDOW_NAME, value2);
//}
//
//int main()
//{
//	//UseImg("juzi.jpg");
//	//UseCapture();
//
//	namedWindow(WINDOW_NAME, 1);
//	createTrackbar("图片：", WINDOW_NAME, &barValue, barValueMax, OnTrackbar);
//	OnTrackbar(barValue, 0);
//
//	waitKey(0);
//	return 0;
//}
//
//
