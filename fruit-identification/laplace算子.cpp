#include <laplaceËã×Ó.h>

Mat EdgeLaplace::Process(Mat inputImg)
{
	Mat grad_sebel, dst;
	int kernel_size = 3;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	Laplacian(inputImg, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(dst, grad_sebel);
	return grad_sebel;
}
