#include "myclass.h"
# pragma execution_character_set("UTF-8")

MyClass::MyClass(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.label->setText("水果识别系统");
	ui.label_2->setText("识别结果");
	ui.plainTextEdit->appendHtml("<h1>水果识别系统</h1>");
	ui.textEdit->setText("输入图片名");
	ui.pushButton->setText("开启摄像头");
	ui.pushButton_2->setText("图片处理");
	ui.pushButton_3->setText("水果识别");
	ui.pushButton_4->setText("打开图片");
	ui.pushButton_5->setText("视频处理");
	ui.comboBox->addItem("二值化");
	ui.comboBox->addItem("均值滤波");
	ui.comboBox->addItem("sebel导数");
	ui.comboBox->addItem("canny边缘检测");
	ui.comboBox->addItem("直方图均衡化");
	ui.comboBox->addItem("laplace算子"); 
	ui.comboBox->addItem("高斯滤波");
	ui.comboBox->addItem("霍夫线性变换"); 

}

MyClass::~MyClass()
{

}

///水果识别
void MyClass::fruitIdentification()
{
	fruitIdentificationFlag = !fruitIdentificationFlag;
}
void MyClass::openImg()
{
	String imgSrc = ui.textEdit->toPlainText() .toStdString();
	srcImage = imread(imgSrc, 1);
	imshow("srcImage", srcImage);
	cvtColor(srcImage, srcImage, CV_BGR2GRAY);
}
//图像处理
void MyClass::imgProcess()
{
	cv::destroyWindow("srcImage");
	String selectModel = ui.comboBox->currentText().toStdString();
	IP = SimpleFactory::Create(selectModel);
	processImg = IP->Process(srcImage);
	imshow("processImg", processImg);
}
//视频处理
void MyClass::vadioProcess()
{
	vadioProcessFlag = !vadioProcessFlag;
}

//开启摄像头
void MyClass::openCapture()
{
	ui.plainTextEdit->appendHtml("摄像头启动中...");
	VideoCapture capture;
	capture.open(0);
	if (capture.isOpened())
	{
		ui.plainTextEdit->appendHtml("摄像头已开启！");
		for (;;)
		{
			capture >> srcVadio;
			if (srcVadio.empty())
				break;
			drawText(srcVadio, "hello");

			if (fruitIdentificationFlag)
			{
				for (int i = 0; i < 4; i++)//四次颜色检查
				{
					whiteNum = 0;
					colorJudgeImg = colorJudge(srcVadio, fruitColor[i][0], fruitColor[i][1]);
					int rows = colorJudgeImg.rows;
					int cols = colorJudgeImg.cols;
					for (int i = 0; i < rows; i++)
					{
						for (int j = 0; j < cols; j++)
						{
							if ((int)colorJudgeImg.ptr<uchar>(i)[j] == 255)
							{
								whiteNum++;
							}
						}
					}
					
					ofstream fout(path);
					if (fout) {
						fout << "白点数目" << QString::number(whiteNum).toStdString() <<"宽度"<< QString::number(width).toStdString() <<"高度"<< QString::number(height) .toStdString()<< endl;
						//fout.close();
					}
					ui.plainTextEdit->appendHtml("白点数目:" + QString::number(whiteNum) + "宽度:" + QString::number(width) + "高度:" + QString::number(height));
					//ui.plainTextEdit->appendHtml(QString::number(height));
					imshow("colorJudge", colorJudgeImg);
					if (height > 20 && whiteNum > 10000)
					{
						if (i == 3)
						{
							ui.label_2->setText("苹果"); break;
						}
						else if (width > 350 || height > 350)
						{
							ui.label_2->setText("香蕉");
						}
						else if (width > 150 && height < 100 && height > 70)
						{
							ui.label_2->setText("橘子");
						}
						else if (width < 130 && height < 130)
						{
							ui.label_2->setText("枣");
						}
						height = 0;
						xiangjiao = 0;
						juzi = 0;
						zao = 0;
						pingguo = 0;
					}

					waitKey(300);
				}
			}
			else 
			{
				cv::destroyWindow("colorJudge"); 
			}

			if (vadioProcessFlag)
			{
				cvtColor(srcVadio,srcVadio, CV_BGR2GRAY);
				selectModel = ui.comboBox->currentText().toStdString();
				IP = SimpleFactory::Create(selectModel);
				processVadio = IP->Process(srcVadio);
				imshow("processImg", processVadio);
			}
			else
			{
				cv::destroyWindow("processImg");
			}
			imshow("srcVadio", srcVadio);
			if (waitKey(10) >= 0)
				break;
		}
	}
	else
	{
		ui.plainTextEdit->appendHtml("没有摄像头！");
		srcImage = Mat::zeros(480, 640, CV_8UC1);
		drawText(srcImage, "hello");
		imshow("Sample", srcImage);
		waitKey(0);
	}
}

//颜色识别
Mat MyClass::colorJudge(Mat inputImg,int low,int high)
{
	int iLowH = low;
	int iHighH = high;
	int iLowS = 90;
	int iHighS = 255;
	int iLowV = 90;
	int iHighV = 255;
	Mat imgHSV;
	vector<Mat> hsvSplit;
	cvtColor(inputImg, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV  

												  //因为我们读取的是彩色图，直方图均衡化需要在HSV空间做  
	split(imgHSV, hsvSplit);
	equalizeHist(hsvSplit[2], hsvSplit[2]);
	merge(hsvSplit, imgHSV);
	Mat outputImg;

	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), outputImg); //Threshold the image  
																					 //开操作 (去除一些噪点)  
	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphologyEx(outputImg, outputImg, MORPH_OPEN, element);
	
	//闭操作 (连接一些连通域)  
	morphologyEx(outputImg, outputImg, MORPH_CLOSE, element);
	
	int rows = outputImg.rows;
	int cols = outputImg.cols;

	vector <Point> points;

	for (int i = 0; i < rows; i++)
	{
		int flag = 0;
		for (int j = 0; j < cols; j++)
		{
			if ((int)outputImg.ptr<uchar>(i)[j] == 255)
			{
				flag++;
				if (flag == 1)
				{
					Point point;
					point.x = j;
					point.y = i;
					points.push_back(point);
				}
			}
			if (flag > 0 && (int)outputImg.ptr<uchar>(i)[j] == 0)
			{
				Point point;
				point.x = j;
				point.y = i;
				points.push_back(point);
				break;
			}
		}
	}

	RotatedRect box = minAreaRect(Mat(points));
	Point2f vertex[4];
	box.points(vertex);

	width = (int)vertex[0].x - (int)vertex[1].x;
	height = (int)vertex[0].y - (int)vertex[1].y;

	//Mat huidu;
	//cvtColor(inputImg, huidu, CV_BGR2GRAY);

	for (int i = 0; i < 4; i++)
	{
		line(outputImg, vertex[i], vertex[(i + 1) % 4], Scalar(100, 200, 211), 2, LINE_AA);
	}
	return outputImg;
}
