#ifndef IMAGE_SHARPENING_H
#define IMAGE_SHARPENING_H
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>

//const int laplacianMask[] = {-1, -1, -1, -1, 9, -1, -1, -1, -1};
//const int laplacianMask[] = {0, -1, 0, -1, 4, -1, 0, -1, 0};
//const int laplacianMask[] = {-1, -1, -1, -1, 8, -1, -1, -1, -1};
struct ImageSharp{
public:	
	void sharp(cv::Mat &image);
};
#endif