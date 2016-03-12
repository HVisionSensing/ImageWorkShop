#ifndef BEEPS_H
#define BEEPS_H
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>
struct BeepsHorizontalVertical{

};

struct BeepsVerticalHorizontal{

};

struct BeepsProgressive{

};

struct BeepsRegressive{

};

struct Beeps{
public:
	cv::Mat image;
	double rhp;
	double spatialDecay;
	double c;
	double photometricStandardDeviation;
	Beeps();
	void beeps(cv::Mat &image, int iter, double photometricStandardDeviationval, double spatialDecayval);
	void beepsVerticalHorizontal(cv::Mat &image);
	void beepsHorizontalVertical(cv::Mat &image);
	void progressive(cv::Mat &image);
	void regressive(cv::Mat &image);
	void gain(cv::Mat &image);

	void beeps2(cv::Mat &image, int iter, double photometricStandardDeviationval, double spatialDecayval);
	void beepsVerticalHorizontal(double *data, int width, int height);
	void beepsHorizontalVertical(double *data, int width, int height);
	void progressive(double *data, int begin, int length);
	void regressive(double *data, int begin, int length);
	void gain(double *data, int begin, int length);
	void dataCopy(double *data, double *target, int length);
	//int Partition(int *sort, int begin, int end);
	///void QuickSort(int *sort, int begin, int end);
};
#endif