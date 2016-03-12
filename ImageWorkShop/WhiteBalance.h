#ifndef WHITEBALANCE_H
#define WHITEBALANCE_H
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>
class WhiteBalance{
public:
	void perfectReflact(cv::Mat &image, int ratio);
private:
	//double getDis(cv::Point &a, cv::Point &b);
	//void generateModel(cv::Mat &model, double variable);
	//double getLongestDis(cv::Point& center, int height, int width);

};
#endif