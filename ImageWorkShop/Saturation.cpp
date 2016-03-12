#include "Saturation.h"

void Saturation::saturation(cv::Mat &image, double increment){
	cv::Mat img(image.size(), CV_64F);
	image.copyTo(img);
	int b, g, r, max, min;
	double delta, value, L, S, alpha;
	
	for(int row = 0; row < image.rows; row++){
		for(int col = 0; col < image.cols; col++){
			b = image.at<cv::Vec3b>(row, col).val[0];
			g = image.at<cv::Vec3b>(row, col).val[1];
			r = image.at<cv::Vec3b>(row, col).val[2];

			max = cv::max(b, cv::max(g, r));
			min = cv::min(b, cv::min(g, r));

			delta = (max - min) * 1.0 / 255;
			if(delta == 0)
				continue;
			value = (max + min) * 1.0 / 255;
			L = value / 2;
			if(L < 0.5)
				S = delta / value;
			else
				S = delta / (2 - value);

			if(increment + S >= 1)
				alpha = S;
			else 
				alpha = 1 - increment;

			alpha = 1 / alpha - 1;

			image.at<cv::Vec3b>(row, col).val[0] = b + (b - L * 255) * alpha;
			image.at<cv::Vec3b>(row, col).val[1] = g + (g - L * 255) * alpha;
			image.at<cv::Vec3b>(row, col).val[2] = r + (r - L * 255) * alpha;
		}
	}
}