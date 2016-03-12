#include "ImageSharpening.h"
const int laplacianMask[] = {0, -1, 0, -1, 5, -1, 0, -1, 0};
//const int laplacianMask[] = {-1, -1, -1, -1, 9, -1, -1, -1, -1};
void ImageSharp::sharp(cv::Mat &cvimage){
	cv::Mat image(cvimage.size(), CV_8UC3);
	cvimage.copyTo(image);
	int radius = 1;
	for(int row = radius; row < image.size().height - radius; row++){
		for(int col = radius; col < image.size().width - radius; col++){
			int result[3] = {0};
			int k = 0;
			for(int i = -radius; i < radius + 1; i++){
				for(int j = -radius; j < radius + 1; j++){
					result[0] += image.at<cv::Vec3b>(row + i, col + j).val[0] * laplacianMask[k];
					result[1] += image.at<cv::Vec3b>(row + i, col + j).val[1] * laplacianMask[k];
					result[2] += image.at<cv::Vec3b>(row + i, col + j).val[2] * laplacianMask[k];
					k++;
				}
			}
			for(int i = 0; i < 3; i++){
				if(result[i] < 0) result[i] = 0;
				else if(result[i] > 255) result[i] = 255;
			}
			cvimage.at<cv::Vec3b>(row, col).val[0] =  result[0];
			cvimage.at<cv::Vec3b>(row, col).val[1] =  result[1];
			cvimage.at<cv::Vec3b>(row, col).val[2] =  result[2];
		}
	}
}