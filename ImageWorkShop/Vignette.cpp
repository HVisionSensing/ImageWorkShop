#include "Vignette.h"
#include <cmath>
void Vignette::vignette(cv::Mat &img, double variable){
	cv::Mat model(img.size(), CV_64F);
    generateModel(model, variable);

	cv::Mat labImg(img.size(), CV_8UC3);

    cv::cvtColor(img, labImg, CV_BGR2Lab);
    for (int row = 0; row < labImg.size().height; row++)
    {
        for (int col = 0; col < labImg.size().width; col++)
        {
            cv::Vec3b value = labImg.at<cv::Vec3b>(row, col);
            value.val[0] *= model.at<double>(row, col);
            labImg.at<cv::Vec3b>(row, col) =  value;
        }
    }
    cv::cvtColor(labImg, img, CV_Lab2BGR);
}

double Vignette::getDis(cv::Point &a, cv::Point &b){
	return sqrt(pow((double) (a.x - b.x), 2) + pow((double) (a.y - b.y), 2));
}

void Vignette::generateModel(cv::Mat &model, double variable){
	cv::Point center = cv::Point(model.size().width/2, model.size().height/2);
	double longestDis = getLongestDis(center, model.size().height, model.size().width);
    for (int i = 0; i < model.rows; i++)
    {
        for (int j = 0; j < model.cols; j++)
        {
            double temp = getDis(center, cv::Point(j, i)) / (longestDis * variable);
            double temp_s = pow(cos(temp), 4);
            model.at<double>(i, j) = temp_s;
        }
    }
}

double Vignette::getLongestDis(cv::Point& center, int height, int width){
	std::vector<cv::Point> point(4);
	point[0] = cv::Point(0,0);
	point[0] = cv::Point(0,height);
	point[0] = cv::Point(width,0);
	point[0] = cv::Point(width,height);
	double result = 0;
	for(int i = 0; i < 4; i++){
		double d = getDis(point[i], center);
		if(d > result)
			result = d;
	}
	return result;
}
