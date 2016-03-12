#include "WhiteBalance.h"
void WhiteBalance::perfectReflact(cv::Mat &image, int ratio){
	int *record = new int[image.rows * image.cols];
	for(int i = 0; i < image.rows * image.cols; i++)
		record[i] = 0;
	for(int row = 0; row < image.rows; row++){
		for(int col = 0; col < image.cols; col++){
			record[row * image.cols + col] = image.at<cv::Vec3b>(row, col).val[0] + image.at<cv::Vec3b>(row, col).val[1] + image.at<cv::Vec3b>(row, col).val[2];
		}
	}

	int histogram[255 * 3 + 1];
	for(int i = 0; i < 255 * 3; i++)
		histogram[i] = 0;
	for(int i = 0; i < image.rows * image.cols; i++)
		histogram[record[i]]++;
	int sum = 0;
	int threshold;
	for(int i = 255 * 3 - 1; i > 0; i--){
		sum += histogram[i];
		if(sum > image.rows * image.cols * ratio / 100){
			threshold = i;
			break;
		}
	}
	double avgB = 0, avgG = 0, avgR = 0;
	int amount = 0;
	for(int row = 0; row < image.rows; row++){
		for(int col = 0; col < image.cols; col++){
			if(record[row * image.cols + col] > threshold){
				avgB += image.at<cv::Vec3b>(row, col).val[0];
				avgG += image.at<cv::Vec3b>(row, col).val[1];
				avgR += image.at<cv::Vec3b>(row, col).val[2];
				amount++;
			}
		}
	}

	avgB /= amount;
	avgG /= amount;
	avgR /= amount;

	int r, g, b;
	int max = 255;
	for(int row = 0; row < image.rows; row++){
		for(int col = 0; col < image.cols; col++){
			if(avgB > 0){
				b = image.at<cv::Vec3b>(row, col).val[0];
				b = b * max / avgB;
				if(b > 255) b = 255;
				else if(b < 0) 
					b = 0;
				image.at<cv::Vec3b>(row, col).val[0] = b;
			}
			if(avgG > 0){
				g = image.at<cv::Vec3b>(row, col).val[1];
				g = g * max / avgG;
				if(g > 255) g = 255;
				else if(g < 0) 
					g = 0;
				image.at<cv::Vec3b>(row, col).val[1] = g;
			}
			if(avgR > 0){
				r = image.at<cv::Vec3b>(row, col).val[2];
				r = r * max / avgR;
				if(r > 255) r = 255;
				else if(r < 0) 
					r = 0;
				image.at<cv::Vec3b>(row, col).val[2] = r;
			}
		}
	}

	delete record;
}