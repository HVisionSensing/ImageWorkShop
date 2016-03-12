#include "MedianFilter.h"
void MedianFilter::medianFilter(cv::Mat &cvimage, int radius){
	std::vector<int> recordB((2 * radius + 1) * (2 * radius + 1));
	std::vector<int> recordG((2 * radius + 1) * (2 * radius + 1));
	std::vector<int> recordR((2 * radius + 1) * (2 * radius + 1));

	cv::Mat image(cvimage.size(), CV_8UC3);
	cvimage.copyTo(image);
	for(int row = 0; row < image.size().height; row++){
		for(int col = 0; col < image.size().width; col++){
			int k = 0;
			for(int i = -radius; i < radius + 1; i++){
				for(int j = -radius; j < radius + 1; j++){
					if(row + i < 0 || row + i > image.size().height - 1 || col + j < 0 || col + j > image.size().width - 1)
						continue;
					recordB[k] = image.at<cv::Vec3b>(row + i, col + j).val[0];
					recordG[k] = image.at<cv::Vec3b>(row + i, col + j).val[1];
					recordR[k] = image.at<cv::Vec3b>(row + i, col + j).val[2];
					k++;
				}
			}
			QuickSort(&recordB[0], 0 , k);
			QuickSort(&recordG[0], 0 , k);
			QuickSort(&recordR[0], 0 , k);
			cvimage.at<cv::Vec3b>(row, col).val[0] =  recordB[k / 2];
			cvimage.at<cv::Vec3b>(row, col).val[1] =  recordG[k / 2];
			cvimage.at<cv::Vec3b>(row, col).val[2] =  recordR[k / 2];
		}
	}
	recordB.clear();
	recordG.clear();
	recordR.clear();

}

void MedianFilter::QuickSort(int *sort, int begin, int end)
{
	if(begin >= end) return;
	int mid = Partition(sort, begin, end);
	QuickSort(sort, begin, mid - 1);
	QuickSort(sort, mid + 1, end);
}
int MedianFilter::Partition(int *sort, int begin, int end){
	unsigned c;
	int i = begin - 1;
	for(int j = begin; j < end; j++){
		if(sort[j] <= sort[end]){
			i++;
			c = sort[i];
			sort[i] = sort[j];
			sort[j] = c;
		}
	}
	c = sort[i + 1];
	sort[i + 1] = sort[end];
	sort[end] = c;
	return i + 1;
}