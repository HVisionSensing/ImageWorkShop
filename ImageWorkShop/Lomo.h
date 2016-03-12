#ifndef LOMO_H
#define LOMO_H
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>
enum CHANNEL{RGB, BLUE, GREEN, RED};
class Lomo{
public:
	Lomo();
	cv::Mat lomoImage;
	unsigned char Table[256];
	unsigned char LinearTable[256];
	void lomoStyle1(cv::Mat &image);
	void Sepia(cv::Mat &image);
	void Sepia2(cv::Mat &image);
	void Carving(cv::Mat &image);
	void Pencil(cv::Mat &image);
	void Curve(cv::Mat &image, unsigned char * TableB, unsigned char * TableG, unsigned char * TableR);
	void GetLevelTable(unsigned char * Table, unsigned char InputLeftLimit, unsigned char InputMiddle, unsigned char InputRightLimit, unsigned char OutputLeftLimit , unsigned char OutputRightLimit);
private:
	//unsigned char TableB[256];
	//unsigned char TableR[256];
	//unsigned char TableG[256];
	
	
	void Desaturate(cv::Mat &image);
	
	void Level(cv::Mat &image, CHANNEL channel, unsigned char InputLeftLimit, unsigned char InputMiddle, unsigned char InputRightLimit, unsigned char OutputLeftLimit , unsigned char OutputRightLimit);
};
#endif