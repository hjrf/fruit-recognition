#ifndef MYCLASS_H
#define MYCLASS_H

#include <QtWidgets/QMainWindow>
#include "ui_myclass.h"
#include<简单工厂.h>

class MyClass : public QMainWindow
{
	Q_OBJECT

public:
	MyClass(QWidget *parent = 0);
	~MyClass();

private:
	Ui::MyClassClass ui;

private:
	Mat colorJudge(Mat inputImg, int low = 75, int high = 130);

private slots:
	void fruitIdentification();

private slots:
	 void openCapture();

private slots:
	 void openImg();

private slots:
	 void imgProcess();

private slots:
	 void vadioProcess(); 
private:	
	char * path = "D:\\log.txt";
	ImgProcess * IP;
	Mat srcImage,srcVadio;
	Mat processImg, processVadio;
	Mat colorJudgeImg;
	String selectModel;
	bool vadioProcessFlag = false;
	bool fruitIdentificationFlag = false;
	int whiteNum = 0;
	int width,height;
	int judgeNum = 20,judgeCount = 0;
	int xiangjiao, juzi, zao, pingguo;
	int fruitColor[4][2]{ {17,37} ,{17,27},{26,38} ,{160,179} };  //香蕉，橘子，枣
};





#endif // MYCLASS_H
