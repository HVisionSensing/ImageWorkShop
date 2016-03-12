#ifndef PATCH_MATCH_H
#define PATCH_MATCH_H
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>

#define XY_TO_INT(x, y) (((y)<<12)|(x))
#define INT_TO_X(v) ((v)&((1<<12)-1))
#define INT_TO_Y(v) ((v)>>12)

#define patch_w 18

class BITMAP { public:
  int w, h;
  int *data;
  BITMAP(int w_, int h_) :w(w_), h(h_) { 
	  data = new int[w*h]; 
	  for(int i = 0; i < w*h; i++)
		  data[i] = 0;
  }
  ~BITMAP() { delete[] data; }
  int *operator[](int y) { return &data[y*w]; }
};

class PatchMatch{
public:
	int holeRadius;
	int targetRadius;
	int targetCount;
	int sourceRadius;

	char* sourceMask;
	char* targetMask;
	PatchMatch(){
		holeRadius = 10;
		targetRadius = 10;
		sourceRadius = 10;
		targetMask = NULL;
		sourceMask = NULL;
	}
	void patchmatch(cv::Mat &a, cv::Mat &b, BITMAP *ann, BITMAP &annd);
	void patchmatch(cv::Mat &a, cv::Mat &b, BITMAP *&ann, BITMAP *&annd);
	void vote(cv::Mat &target, cv::Mat &source, BITMAP *ann);
	void zeroMask(int length, bool newImage);
	void addToTargetMask(int ax, int ay, int rows, int cols);
	void addToSourceMask(int ax, int ay, int rows, int cols);
	void updateSourceMask(int rows, int cols);
	void getSourceImageFromSourceMask(cv::Mat &image, cv::Mat &sourceImage);
	void addMaskToImage(cv::Mat &image);
	bool checkTargetMask(int length);
	bool checkSourceMask(int length);
private:
	int dist(cv::Mat &a, cv::Mat &b, int ax, int ay, int bx, int by, int cutoff);
	void improve_guess(cv::Mat &a, cv::Mat &b, int ax, int ay, int &xbest, int &ybest, int &dbest, int bx, int by);
	


};
#endif