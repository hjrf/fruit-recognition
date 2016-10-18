//#include "简单工厂.h"
//int WINDOW_WIDTH = 300;
//void DrawEllipes(Mat img, double angle);
//int main()
//{
//	Mat image = imread("pingguo.jpg", 1);
//	DrawEllipes(image, 5.0);
//	imshow("画线", image);
//	waitKey(0);
//	return 0;
//}
//void DrawEllipes(Mat img, double angle)
//{
//	int thickness = 2;
//	int lineType = 8;
//	ellipse(img,
//		Point(WINDOW_WIDTH / 2, WINDOW_WIDTH),
//		Size(WINDOW_WIDTH / 4, WINDOW_WIDTH / 16),
//		angle,
//		0,
//		360,
//		Scalar(225, 129, 0),
//		thickness,
//		lineType);
//}