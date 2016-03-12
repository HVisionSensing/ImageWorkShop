#include "Lomo.h"
#include <cmath>
Lomo::Lomo(){
	for(int i = 0; i < 256; i++)
		LinearTable[i] = i;
}
void Lomo::Sepia(cv::Mat &image){
	//cv::Mat img(image.size(), CV_64F);
	//image.copyTo(img);
	int bb, bg, br;
	int mb, mg, mr;
	for(int row = 0; row < image.rows; row++){
		for(int col = 0; col < image.cols; col++){
			bb = image.at<cv::Vec3b>(row, col).val[0];
			bg = image.at<cv::Vec3b>(row, col).val[1];
			br = image.at<cv::Vec3b>(row, col).val[2];
			mr = (101 * br + 197 * bg + 48 * bb) >> 8; 
			mg = (89 * br + 176 * bg + 43 * bb) >> 8; 
			mb = (70 * br + 137 * bg + 34 * bb) >> 8; 
			if(mr > 255) mr = 255;
			if(mr < 0) mr = 0;
			if(mg > 255) mg = 255;
			if(mg < 0) mg = 0;
			if(mb > 255) mb = 255;
			if(mb < 0) mb = 0;
			image.at<cv::Vec3b>(row, col).val[0] = mb;
			image.at<cv::Vec3b>(row, col).val[1] = mg;
			image.at<cv::Vec3b>(row, col).val[2] = mr;
		}
	}
}

void Lomo::Carving(cv::Mat &image){
	cv::Mat img(image.size(), CV_64F);
	image.copyTo(img);
	int bb, bg, br;
	for(int row = 0; row < image.rows; row++){
		for(int col = 0; col < image.cols; col++){
			if(row + 1 == image.rows)
				continue;
			bb = image.at<cv::Vec3b>(row, col).val[0] - image.at<cv::Vec3b>(row + 1, col).val[0] + 127;
			bg = image.at<cv::Vec3b>(row, col).val[1] - image.at<cv::Vec3b>(row + 1, col).val[1] + 127;
			br = image.at<cv::Vec3b>(row, col).val[2] - image.at<cv::Vec3b>(row + 1, col).val[2] + 127;
			if(br > 255) br = 255;
			if(br < 0) br = 0;
			if(bg > 255) bg = 255;
			if(bg < 0) bg = 0;
			if(bb > 255) bb = 255;
			if(bb < 0) bb = 0;
			image.at<cv::Vec3b>(row, col).val[0] = bb;
			image.at<cv::Vec3b>(row, col).val[1] = bg;
			image.at<cv::Vec3b>(row, col).val[2] = br;
		}
	}
}

void Lomo::Pencil(cv::Mat &image){
	cv::Mat gray(image.size(), CV_64F);
	cv::Mat img(image.size(), CV_64F);
	//cv::Mat img;
	int gray_sv = 30;
	cv::cvtColor(image, gray, CV_BGR2GRAY);
	cv::cvtColor(gray, img, CV_GRAY2BGR);
	int v1, v2, v3, v4, v5, v6, v7, v8;
	for(int row = 0; row < image.rows; row++){
		for(int col = 0; col < image.cols; col++){
			if(row - 1 < 0 || row + 1 >= image.rows || col - 1 < 0 || col + 1 >= image.cols){
				image.at<cv::Vec3b>(row, col).val[0] = img.at<cv::Vec3b>(row, col).val[0];
				image.at<cv::Vec3b>(row, col).val[1] = img.at<cv::Vec3b>(row, col).val[1];
				image.at<cv::Vec3b>(row, col).val[2] = img.at<cv::Vec3b>(row, col).val[2];
			}
			else{
				v1 = abs(img.at<cv::Vec3b>(row, col).val[0] - img.at<cv::Vec3b>(row + 1, col).val[0]);
				v2 = abs(img.at<cv::Vec3b>(row, col).val[0] - img.at<cv::Vec3b>(row + 1, col + 1).val[0]);
				v3 = abs(img.at<cv::Vec3b>(row, col).val[0] - img.at<cv::Vec3b>(row + 1, col - 1).val[0]);
				v4 = abs(img.at<cv::Vec3b>(row, col).val[0] - img.at<cv::Vec3b>(row - 1, col).val[0]);
				v5 = abs(img.at<cv::Vec3b>(row, col).val[0] - img.at<cv::Vec3b>(row - 1, col + 1).val[0]);
				v6 = abs(img.at<cv::Vec3b>(row, col).val[0] - img.at<cv::Vec3b>(row - 1, col - 1).val[0]);
				v7 = abs(img.at<cv::Vec3b>(row, col).val[0] - img.at<cv::Vec3b>(row, col + 1).val[0]);
				v8 = abs(img.at<cv::Vec3b>(row, col).val[0] - img.at<cv::Vec3b>(row, col - 1).val[0]);
				if(v1 > gray_sv || v2 > gray_sv || v3 > gray_sv || v4 > gray_sv || v5 > gray_sv || v6 > gray_sv || v7 > gray_sv || v8 > gray_sv){
					image.at<cv::Vec3b>(row, col).val[0] = 0;
					image.at<cv::Vec3b>(row, col).val[1] = 0;
					image.at<cv::Vec3b>(row, col).val[2] = 0;
				}
				else{
					image.at<cv::Vec3b>(row, col).val[0] = 255;
					image.at<cv::Vec3b>(row, col).val[1] = 255;
					image.at<cv::Vec3b>(row, col).val[2] = 255;
				}
			}
		}
	}
}

void Lomo::Desaturate(cv::Mat &image)
{
	int min, max, mean;
	int b, g, r;
	for(int row = 0; row < image.rows; row++){
		for(int col = 0; col < image.cols; col++){
			if(row + 1 == image.rows)
				continue;
			b = image.at<cv::Vec3b>(row, col).val[0];
			g = image.at<cv::Vec3b>(row, col).val[1];
			r = image.at<cv::Vec3b>(row, col).val[2];
			if(b > g){
				max = b;
				min = g;
			}
			else{
				max = g; 
				min = b;
			}
			if(r > max)
				max = r;
			if(r < min)
				min = r;

			mean = (max + min) >> 1;

			image.at<cv::Vec3b>(row, col).val[0] = mean;
			image.at<cv::Vec3b>(row, col).val[1] = mean;
			image.at<cv::Vec3b>(row, col).val[2] = mean;
		}
	}
}

void Lomo::Sepia2(cv::Mat &image){
	Desaturate(image);
	Level(image, BLUE, 0, 152, 255, 0, 255);
	Level(image, RED, 0, 101, 255, 0, 255);
}
void Lomo::GetLevelTable(unsigned char * Table, unsigned char InputLeftLimit, unsigned char InputMiddle, unsigned char InputRightLimit, unsigned char OutputLeftLimit , unsigned char OutputRightLimit)
{
    if (InputLeftLimit > 253) InputLeftLimit = 253;
    if (InputLeftLimit < 0)    InputLeftLimit = 0;
    if (InputRightLimit > 255)InputRightLimit = 255;
    if (InputRightLimit < 2) InputRightLimit = 2;
    if (InputMiddle > 254)InputMiddle = 254;
    if (InputMiddle < 1)InputMiddle = 1;
    if (InputMiddle > InputRightLimit)InputMiddle = InputRightLimit - 1;
    if (InputMiddle < InputLeftLimit)InputMiddle = InputLeftLimit + 1;
    if (OutputLeftLimit < 0)OutputLeftLimit = 0;
    if (OutputLeftLimit > 255)OutputLeftLimit = 255;
    if (OutputRightLimit < 0)OutputRightLimit = 0;
    if (OutputRightLimit > 255)OutputRightLimit = 255;

    for (int Index = 0; Index <= 255; Index++)
    {
        double Temp = Index - InputLeftLimit;
        if (Temp < 0) 
        {
            Temp = OutputLeftLimit;
        }
        else if (Temp + InputLeftLimit > InputRightLimit)
        {
            Temp = OutputRightLimit;
        }
        else 
        {
            double Gamma = log(0.5) / log((double)(InputMiddle - InputLeftLimit) / (InputRightLimit - InputLeftLimit));
            Temp = OutputLeftLimit + (OutputRightLimit - OutputLeftLimit) * pow((Temp / (InputRightLimit - InputLeftLimit)), Gamma);
        }
        if (Temp > 255)
            Temp = 255;
        else if (Temp < 0)
            Temp = 0;
        Table[Index] = Temp;
    }
}

void Lomo::Curve(cv::Mat &image, unsigned char * TableB, unsigned char * TableG, unsigned char * TableR)
{
	cv::Mat img(image.size(), CV_64F);
	image.copyTo(img);

	for(int row = 0; row < image.rows; row++){
		for(int col = 0; col < image.cols; col++){
			image.at<cv::Vec3b>(row, col).val[0] = TableB[img.at<cv::Vec3b>(row, col).val[0]];
			image.at<cv::Vec3b>(row, col).val[1] = TableG[img.at<cv::Vec3b>(row, col).val[1]];
			image.at<cv::Vec3b>(row, col).val[2] = TableR[img.at<cv::Vec3b>(row, col).val[2]];
		}
	}
}

void Lomo::Level(cv::Mat &image, CHANNEL channel, unsigned char InputLeftLimit, unsigned char InputMiddle, unsigned char InputRightLimit, unsigned char OutputLeftLimit , unsigned char OutputRightLimit)
{
    GetLevelTable(Table, InputLeftLimit,InputMiddle,InputRightLimit,OutputLeftLimit,OutputRightLimit);
	switch(channel){
	case RGB:
		Curve(image,Table,Table,Table);
		break;
	case BLUE:
		Curve(image,Table,LinearTable,LinearTable);
		break;
	case GREEN:
		Curve(image,LinearTable,Table,LinearTable);
		break;
	case RED:
		Curve(image,LinearTable,LinearTable,Table);
		break;
	default:
		break;
	}
}

void Lomo::lomoStyle1(cv::Mat &image){
	unsigned char TB[256];
	unsigned char TG[256];
	unsigned char TR[256];
	GetLevelTable(TR, 0, 54, 255, 0, 255);
	GetLevelTable(TG, 0, 88, 255, 0, 255);
	GetLevelTable(TB, 0, 155, 255, 0, 255);
	Curve(image, TB, TG, TR);
}