#include "简单工厂.h"

void drawText(Mat & inputImg, string text);

int UseCapture(string modelName)
{
	cout << "Built with OpenCV " << CV_VERSION << endl;
	Mat image,outputImg;
	char* window_name = "Sobel Demo - Simple Edge Detector";
	VideoCapture capture;
	capture.open(0);
	if (capture.isOpened())
	{
		cout << "Capture is opened" << endl;
		for (;;)
		{
			capture >> image;
			if (image.empty())
				break;

			drawText(image,"hello");
		
			ImgProcess * IP_HuiDu = SimpleFactory::Create("灰度化");
			Mat huidu = IP_HuiDu->Process(image);

			ImgProcess * IP = SimpleFactory::Create(modelName);
			
			Mat outputImg = IP->Process(huidu);
			imshow("hjr",outputImg);

			if (waitKey(10) >= 0)
				break;
		}
	}
	else
	{
		cout << "No capture" << endl;
		image = Mat::zeros(480, 640, CV_8UC1);
		drawText(image,"hello");
		imshow("Sample", image);
		waitKey(0);
	}
	return 0;
}


