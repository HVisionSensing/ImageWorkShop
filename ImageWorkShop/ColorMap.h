#ifndef COLOR_MAP_H
#define COLOR_MAP_H
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>
struct ColorMap{
public:
	cv::Mat faceMask;
	ColorMap();
	int width;
	int height;
	
	void computeFractal(cv::Mat gray, int radius);
};
#endif