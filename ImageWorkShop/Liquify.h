#ifndef LIQUIFY_H
#define LIQUIFY_H
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>
class Liquify{
public:
	char *mask;
	int radius;
	int centerX, centerY;
	cv::Mat image;
	Liquify();
	~Liquify();
	void genMask();
	void addCircle(cv::Mat &img);
	void liquify(cv::Mat &image, int strength, int pointX, int pointY);
private:
};
#endif