//摄像头：640x480像素
//H:0-180
//S:0-255
//V:0-255
//Orange  0-22  
//Yellow 22 - 38
//Green 38 - 75
//Blue 75 - 130
//Violet 130 - 160
//Red 160 - 179
#include"简单工厂.h"
void drawText(Mat & inputImg, string text);
Rect rect(30, 30, 200, 100);
int drawKey = 0;
int main(int argc, char** argv)
{
	char * path = "D:\\1.txt";
	ofstream fout(path);
	if (fout) {
		fout << "侯金瑞" << endl;
		//fout.close();
	}
	VideoCapture cap(0); //capture the video from web cam  

	if (!cap.isOpened())  // if not success, exit program  
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"  

	int iLowH = 75;
	int iHighH = 130;

	int iLowS = 90;
	int iHighS = 255;

	int iLowV = 90;
	int iHighV = 255;

	//Create trackbars in "Control" window  
	cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)  
	cvCreateTrackbar("HighH", "Control", &iHighH, 179);

	cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)  
	cvCreateTrackbar("HighS", "Control", &iHighS, 255);

	cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)  
	cvCreateTrackbar("HighV", "Control", &iHighV, 255);

	while (true)
	{
		Mat imgOriginal;

		bool bSuccess = cap.read(imgOriginal); // read a new frame from video  

		if (!bSuccess) //if not success, break loop  
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		Mat imgHSV;
		vector<Mat> hsvSplit;
		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV  

													  //因为我们读取的是彩色图，直方图均衡化需要在HSV空间做  
		split(imgHSV, hsvSplit);
		equalizeHist(hsvSplit[2], hsvSplit[2]);
		merge(hsvSplit, imgHSV);
		Mat imgBlue;
		Mat imgRed;
		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgBlue); //Threshold the image  
		inRange(imgHSV, Scalar(160, iLowS, iLowV), Scalar(179, iHighS, iHighV), imgRed); //Threshold the image  
																									  //开操作 (去除一些噪点)  
		Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
		morphologyEx(imgBlue, imgBlue, MORPH_OPEN, element);
		morphologyEx(imgRed, imgRed, MORPH_OPEN, element);

		//闭操作 (连接一些连通域)  
		morphologyEx(imgBlue, imgBlue, MORPH_CLOSE, element);
		morphologyEx(imgRed, imgRed, MORPH_CLOSE, element);
		int rows = imgBlue.rows;
		int cols = imgBlue.cols;
		vector <Point> points_blue;
		vector <Point> points_red;

		for (int i = 0; i < rows; i++)
		{
			int flag = 0;
			for (int j = 0; j < cols; j++)
			{
				if ((int)imgBlue.ptr<uchar>(i)[j] == 255)
				{
					drawKey = 1;
					if (drawKey == 1)
					{
						//这里检测放入的水果
						drawText(imgBlue, "blue");
					}
					flag++;
					if (flag == 1)
					{
						Point point;
						point.x = j;
						point.y = i;
						points_blue.push_back(point);
					}
				}
				if (flag > 0 && (int)imgBlue.ptr<uchar>(i)[j] == 0)
				{
					Point point;
					point.x = j;
					point.y = i;
					points_blue.push_back(point);
					break;
				}
			}
		}

		for (int i = 0; i < rows; i++)
		{
			int flag = 0;
			for (int j = 0; j < cols; j++)
			{
				if ((int)imgRed .ptr<uchar>(i)[j] == 255)
				{
					flag++;
					if (flag == 1)
					{
						Point point;
						point.x = j;
						point.y = i;
						points_red.push_back(point);
					}
				}
				if (flag > 0 && (int)imgRed.ptr<uchar>(i)[j] == 0)
				{
					Point point;
					point.x = j;
					point.y = i;
					points_red.push_back(point);
					break;
				}
			}
		}

		RotatedRect box_blue = minAreaRect(Mat(points_blue));
		Point2f vertex_blue[4];
		RotatedRect box_red = minAreaRect(Mat(points_red));
		Point2f vertex_red[4];
		box_blue.points(vertex_blue);
		box_red.points(vertex_red);
		fout << "[" << (int)vertex_blue[0].x << ":" << (int)vertex_blue[0].y << "]" << "[" << (int)vertex_blue[1].x << ":" << (int)vertex_blue[1].y << "]" << "[" << (int)vertex_blue[2].x << ":" << (int)vertex_blue[2].y << "]" << "[" << (int)vertex_blue[3].x << ":" << (int)vertex_blue[3].y << "]" << endl;
		cout << "[" << (int)vertex_blue[0].x << ":" << (int)vertex_blue[0].y << "]" << "[" << (int)vertex_blue[1].x << ":" << (int)vertex_blue[1].y << "]" << "[" << (int)vertex_blue[2].x << ":" << (int)vertex_blue[2].y << "]" << "[" << (int)vertex_blue[3].x << ":" << (int)vertex_blue[3].y << "]" << endl;
		
		Mat huidu;
		cvtColor(imgOriginal,huidu, CV_BGR2GRAY);
		
		for (int i = 0; i < 4; i++)
		{
			line(imgBlue, vertex_blue[i], vertex_blue[(i + 1) % 4], Scalar(100, 200, 211), 2, LINE_AA);
			line(huidu, vertex_blue[i], vertex_red[(i + 1) % 4], Scalar(100, 200, 211), 2, LINE_AA);
			line(imgRed, vertex_red[i], vertex_red[(i + 1) % 4], Scalar(100, 200, 211), 2, LINE_AA);
		}

		//if ((int)vertex_blue[0].x >0 && (int)vertex_blue[0].x < 640-300 && (int)vertex_blue[1].x >0 && (int)vertex_blue[1].x <480-150)
		//{
		//	rect.x = (int)vertex_blue[0].x;
		//	rect.y = (int)vertex_blue[1].x;
		//	rect.width = 300;
		//	rect.height = 150;
		//}
		if (drawKey == 0)
		{
			drawText(imgBlue, "none");
		}
		Mat imgRect = imgOriginal(rect);
		imshow("蓝色识别", imgBlue); //show the thresholded image  
		//imshow("红色识别", imgRed);
		imshow("原图像", imgOriginal); //show the original image  
		//imshow("图像矩形片段", imgRect);
		
		char key = (char)waitKey(300);
		if (key == 27)
			break;
	}

	return 0;

}
