#ifndef COLOR_TRANSFER_H
#define COLOR_TRANSFER_H
#include <opencv2/opencv.hpp>  
#include <opencv2/core/core.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
using namespace std;  
using namespace cv;  
class Point2D{
public:
	int x;
	int y;
	Point2D(int x_, int y_){
		x = x_;
		y = y_;
	}
};
class ColorTransfer  
{  
private:  
    Mat srcImg;  
    Mat targetImg;  
    Mat srcImg_lab;//CV_32FC3  
    Mat targetImg_lab;//CV_32FC3  
	float *srcD;  
    float *targetD; 
	float *srcL;  
    float *targetL; 
    //Mat result_lab;  
    vector<float> srcMeans;  
    vector<float> srcVariances;  
    vector<float> targetMeans;  
    vector<float> targetVariances;  
    void initialize();
    void computeMeans();
    void computeVariances();
	void computeLocalVariances(Mat &gray, int radius, Mat &result);
	void computeFractal(Mat &gray, int radius, float* &D, float* &L);
	
	void fractalTransfer(Mat &srcImg_32F, Mat &targetImg_32F, Mat &c1, Mat &c2);
public:  
    Mat result;  
    void solve();
	void localVarianceTransfer(Mat &src, Mat &tgt);
      
	ColorTransfer(){}
 
    ColorTransfer(Mat src, Mat target):srcImg(src), targetImg(target)  
    {  
        srcMeans.resize(3, 0.f);  
        srcVariances.resize(3, 0.f);  
        targetMeans.resize(3, 0.f);  
        targetVariances.resize(3, 0.f);  
		srcD = NULL;
		srcL = NULL;
		targetL = NULL; 
		targetD = NULL;
        solve();  
    }  
};  
  


#endif