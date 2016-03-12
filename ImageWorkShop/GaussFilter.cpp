#include "GaussFilter.h"
#include <cmath>
void GaussFilter::gaussFilter(cv::Mat &image, int radius){
	cv::Mat img(image.size(), CV_64F);
	double *kernel = new double[radius * 2 + 1];
	genKernel(kernel, radius);
	image.copyTo(img);

	for(int row = 0; row < image.size().height; row++){
		for(int col = 0; col < image.size().width; col++){
			double r = 0, g = 0, b = 0;
			for(int i = -radius; i <= radius; i++){
				int c_col = col + i;
				if(c_col < 0)
					c_col *= -1;
				else if(c_col > image.size().width - 1)
					c_col = 2 * (image.size().width - 1) - c_col;

				b += image.at<cv::Vec3b>(row, c_col).val[0] * kernel[i + radius];
				g += image.at<cv::Vec3b>(row, c_col).val[1] * kernel[i + radius];
				r += image.at<cv::Vec3b>(row, c_col).val[2] * kernel[i + radius];
			}
			img.at<cv::Vec3b>(row, col).val[0] = (int)b;
			img.at<cv::Vec3b>(row, col).val[1] = (int)g;
			img.at<cv::Vec3b>(row, col).val[2] = (int)r;
		}
	}
	image = img.t();
	image.copyTo(img);

	for(int row = 0; row < image.size().height; row++){
		for(int col = 0; col < image.size().width; col++){
			double r = 0, g = 0, b = 0;
			for(int i = -radius; i <= radius; i++){
				int c_col = col + i;
				if(c_col < 0)
					c_col *= -1;
				else if(c_col > image.size().width - 1)
					c_col = 2 * (image.size().width - 1) - c_col;

				b += image.at<cv::Vec3b>(row, c_col).val[0] * kernel[i + radius];
				g += image.at<cv::Vec3b>(row, c_col).val[1] * kernel[i + radius];
				r += image.at<cv::Vec3b>(row, c_col).val[2] * kernel[i + radius];
			}
			img.at<cv::Vec3b>(row, col).val[0] = (int)b;
			img.at<cv::Vec3b>(row, col).val[1] = (int)g;
			img.at<cv::Vec3b>(row, col).val[2] = (int)r;
		}
	}
	image = img.t();
	delete kernel;
}

void GaussFilter::genKernel(double *kernel, int radius){
	double fx = 0;
	double PI = 3.1415926, e = 2.718281828;
	for(int i = 1; i <= radius; i++)
		fx += i * i * 1.0;
	fx = fx * 2 / (2 * radius + 1);
	for(int i = -radius; i < radius + 1; i++){
		kernel[i + radius] = pow(e, -(radius * radius) / (2 * fx)) / sqrt(2 * PI * fx);
	}
	double sum = 0;
	for(int i = 0; i < radius * 2 + 1; i++){
		sum += kernel[i];
	}
	for(int i = 0; i < radius * 2 + 1; i++){
		kernel[i] /= sum;
	}
}