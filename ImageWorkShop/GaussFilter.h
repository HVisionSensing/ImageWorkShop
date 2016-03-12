#ifndef GAUSSFILTER_H
#define GAUSSFILTER_H
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>
struct GaussFilter{
public:
	void gaussFilter(cv::Mat &image, int radius);
	void genKernel(double *kernel, int radius);
	//int Partition(int *sort, int begin, int end);
	///void QuickSort(int *sort, int begin, int end);
};
#endif