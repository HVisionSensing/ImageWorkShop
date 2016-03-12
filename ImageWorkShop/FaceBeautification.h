#ifndef FACE_BEAUTIFICATION_H
#define FACE_BEAUTIFICATION_H
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>
#include "Beeps.h"
struct FaceBeautification{
public:
	cv::Mat faceMask;

	Beeps myBeeps;
	FaceBeautification();
	void genFaceMask(cv::Mat &image);
	//void genFaceMask2(cv::Mat &image);
	void genKernel(double *kernel, int radius);
	void bilateralFilter(cv::Mat &imagev, double Deviationval, double spatialDecayval);
	void curve(cv::Mat &image, int beta);
	//int Partition(int *sort, int begin, int end);
	///void QuickSort(int *sort, int begin, int end);
};
#endif