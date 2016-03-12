#ifndef MEDIANFILTER_H
#define MEDIANFILTER_H
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>
struct MedianFilter{
public:
	MedianFilter(){}
	void medianFilter(cv::Mat &image, int radius);
	int Partition(int *sort, int begin, int end);
	void QuickSort(int *sort, int begin, int end);
};
#endif