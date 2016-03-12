#include "Liquify.h"	
#include <fstream>
#include <iostream>
using namespace std;
Liquify::Liquify(){
	mask = NULL;
	radius = 20;
	centerX = 0;
	centerY = 0;
	genMask();
}

Liquify::~Liquify(){
	if(mask != NULL)
		delete mask;
}

void Liquify::genMask(){
	if(mask != NULL)
		delete mask;
	mask = new char[(radius * 2 + 1) * (radius * 2 + 1)];
	for(int x = 0; x < radius * 2 + 1; x++){
		for(int y = 0; y < radius * 2 + 1; y++){
			mask[x * (2 * radius + 1) + y] = 1;
		}
	}

	int centerx = radius;
	int centery = radius;
	for(int y = 0; y < centery; y++){
		for(int x = 0; x < centerx - y; x++){
			int r = (x - centerx) * (x - centerx) + (y - centery) * (y - centery);
			if(r > radius * radius){
				mask[y * (2 * radius + 1) + x] = 0;
				mask[y * (2 * radius + 1) + 2 * radius - x] = 0;
				mask[(2 * radius - y) * (2 * radius + 1) + x] = 0;
				mask[(2 * radius - y) * (2 * radius + 1) + 2 * radius - x] = 0;
			}

		}
	}

	for(int y = 0; y < centery + 1; y++){
		for(int x = 0; x < centerx + 1 - y; x++){
			short gx = 1, gy = 1;
			if(x > 0)
				gx = mask[y * (2 * radius + 1) + x] - mask[y * (2 * radius + 1) + x - 1];
			if(y > 0)
				gy = mask[y * (2 * radius + 1) + x] - mask[(y - 1) * (2 * radius + 1) + x];
			if(gx * mask[y * (2 * radius + 1) + x] > 0 || gy * mask[y * (2 * radius + 1) + x] > 0){
				mask[y * (2 * radius + 1) + x] = 2;
				mask[y * (2 * radius + 1) + 2 * radius - x] = 2;
				mask[(2 * radius - y) * (2 * radius + 1) + x] = 2;
				mask[(2 * radius - y) * (2 * radius + 1) + 2 * radius - x] = 2;
				//break;
			}

		}
	}
}


void Liquify::liquify(cv::Mat &image, int strength, int pointX, int pointY){
	cv::Mat img(image.size(), CV_64F);
	image.copyTo(img);
	int vecX = pointX - centerX;
	int vecY = pointY - centerY;
	int newX;
	int newY;
	double dX,dY;

	//const char filename[] = "mytext.txt";
 //   ofstream o_file;
 //   string out_text;
   
    //д
    //o_file.open(filename);
    
    
    double xadd;
	double yadd;


	for(int row = -radius; row < radius + 1; row++){
		for(int col = -radius; col < radius + 1; col++){
			if(centerX + col < 0 || centerY + row < 0 || centerX + col >= image.cols || centerY + row >= image.rows)
				continue;
			if(mask[(row + radius) * (2 * radius + 1) + col + radius] > 0){
				double p = radius * radius - row * row - col * col;
				p = p * 1.0 / (p + radius * radius + 1 * (vecX * vecX + vecY * vecY)); 
				p = p * p;
				dX = col - p * vecX;
				dY = row - p * vecY;
				newX = (int)(col - p * vecX);
				newY = (int)(row - p * vecY);
				if(centerX + newX < 0 || centerY + newY < 0 || centerX + newX >= image.cols || centerY + newY >= image.rows)
					continue;
				xadd = 1;
				yadd = 1;

				if(dX < 0) xadd = -1;
				if(dY < 0) yadd = -1;

				if(centerX + newX + xadd < 0 || centerX + newX + xadd >= image.cols || centerY + newY + yadd < 0 || centerY + newY + yadd >= image.rows)
					continue;

				double x1 = (dX - newX) * xadd;
				double y1 = (dY - newY) * yadd;
				cv::Vec3b cross1 = img.at<cv::Vec3b>(centerY + newY, centerX + newX) * (1 - y1) + img.at<cv::Vec3b>(centerY + newY + yadd, centerX + newX) * y1;
				cv::Vec3b cross2 = img.at<cv::Vec3b>(centerY + newY, centerX + newX + xadd) * (1 - y1) + img.at<cv::Vec3b>(centerY + newY + yadd, centerX + newX + xadd) * y1;
				image.at<cv::Vec3b>(centerY + row, centerX + col) = cross1 * (1 - x1) + cross2 * x1;
				//image.at<cv::Vec3b>(centerY + row, centerX + col) = img.at<cv::Vec3b>(centerY + newY, centerX + newX);

			}
		}
	}
	//o_file.close();
}


void Liquify::addCircle(cv::Mat &img){
	img.rows = image.rows;
	img.cols = image.cols;
	img.dims = image.dims;
	image.copyTo(img);
	for(int row = -radius; row < radius + 1; row++){
		for(int col = -radius; col < radius + 1; col++){
			if(centerX + col < 0 || centerY + row < 0 || centerX + col >= image.cols || centerY + row >= image.rows)
				continue;
			if(mask[(row + radius) * (2 * radius + 1) + col + radius] == 2){	
				img.at<cv::Vec3b>(centerY + row, centerX + col).val[0] = 255;
				img.at<cv::Vec3b>(centerY + row, centerX + col).val[1] = 255;
				img.at<cv::Vec3b>(centerY + row, centerX + col).val[2] = 255;
			}
		}
	}
}