#ifndef VIGNETTE_H
#define VIGNETTE_H
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>
class Vignette{
public:
	void vignette(cv::Mat &model, double variable);
private:
	double getDis(cv::Point &a, cv::Point &b);
	void generateModel(cv::Mat &model, double variable);
	double getLongestDis(cv::Point& center, int height, int width);

};
#endif