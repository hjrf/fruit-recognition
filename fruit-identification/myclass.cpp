#include "myclass.h"
#include<简单工厂.h>
# pragma execution_character_set("UTF-8")
MyClass::MyClass(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.label->setText("水果识别系统");
	ui.plainTextEdit->appendHtml("<h1>水果识别系统</h1>");
	ui.pushButton->setText("开启摄像头");
	ui.pushButton_2->setText("执行");
	ui.comboBox->addItem("二值化");
	ui.comboBox->addItem("均值滤波");
	ui.comboBox->addItem("Sebel导数");
	ui.comboBox->addItem("canny边缘提取");
	ui.comboBox->addItem("直方图均衡化");
}

MyClass::~MyClass()
{
	
}

void MyClass::openCapture()
{
	ui.plainTextEdit->appendHtml("摄像头中...");
	Mat image, outputImg;
	char* window_name = "水果识别视频采集";
	VideoCapture capture;
	capture.open(0);
	if (capture.isOpened())
	{
		ui.plainTextEdit->appendHtml("摄像头已开启！");
		for (;;)
		{
			capture >> image;
			if (image.empty())
				break;

			drawText(image, "hello");

			ImgProcess * IP_HuiDu = SimpleFactory::Create("灰度化");
			Mat huidu = IP_HuiDu->Process(image);

			ImgProcess * IP = SimpleFactory::Create("二值化");

			Mat outputImg = IP->Process(huidu);
			imshow(window_name, outputImg);

			if (waitKey(10) >= 0)
				break;
		}
	}
	else
	{
		ui.plainTextEdit->appendHtml("没有摄像头！");
		image = Mat::zeros(480, 640, CV_8UC1);
		drawText(image, "hello");
		imshow("Sample", image);
		waitKey(0);
	}
}

void MyClass::imgProcess()
{

}
