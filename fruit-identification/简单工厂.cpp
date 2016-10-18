#include"简单工厂.h"

ImgProcess * SimpleFactory::Create(const std::string & model)
{
	if (model == "二值化")
	{
		return new Value2();
	}
	else if (model == "灰度化")
	{
		return new Huidu();
	}
	else if (model == "均值滤波")
	{
		return new LvboAvg();
	}
	else if (model == "sebel导数")
	{
		return new EdgeSebel();
	}
	else if (model == "canny边缘检测")
	{
		return new EdgeCanny();
	}
	else if (model == "直方图均衡化")
	{
		return new HistogramAvg();
	}
	else
	{
		return NULL;
	}

}