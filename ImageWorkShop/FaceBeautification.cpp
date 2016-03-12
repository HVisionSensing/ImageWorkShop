#include "FaceBeautification.h"
#include <cmath>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
FaceBeautification::FaceBeautification(){
	
}
void FaceBeautification::genFaceMask(cv::Mat &image){
	image.copyTo(faceMask);
	int r, g, b, sum, t1, t2, lower, upper;
	for(int row = 0; row < image.size().height; row++){
		for(int col = 0; col < image.size().width; col++){
			b = image.at<cv::Vec3b>(row, col).val[0];
			g = image.at<cv::Vec3b>(row, col).val[1];
			r = image.at<cv::Vec3b>(row, col).val[2];

			faceMask.at<cv::Vec3b>(row, col).val[0] = 0;
			faceMask.at<cv::Vec3b>(row, col).val[1] = 0;
			faceMask.at<cv::Vec3b>(row, col).val[2] = 0;
			if(r - g >= 45){
				if(g > b){
					sum = b + r + g;	
					t1 = 156 * r - 52 * sum;
					t2 = 156 * g - 52 * sum;
					if(t1 * t1 + t2 * t2 >= sum * sum * 39 / 4){
						t1 = 10000 * g * sum;
						lower = -7760 * r * r + 5601 * r * sum + 1766 * sum * sum;
						if(t1 > lower){
							upper = -13767 * r * r + 10743 * r * sum + 1452 *  sum * sum;
							if(t1 < upper){
								faceMask.at<cv::Vec3b>(row, col).val[0] = 255;
								faceMask.at<cv::Vec3b>(row, col).val[1] = 255;
								faceMask.at<cv::Vec3b>(row, col).val[2] = 255;
							}
						}
					}
				}
			}
			

		}
	}
	//cv::imshow("mask", faceMask) ;

}

void FaceBeautification::bilateralFilter(cv::Mat &image, double Deviationval, double spatialDecayval){
	myBeeps.beeps2(image, 1, Deviationval, spatialDecayval);
	//myBeeps.beeps(image, 1, 10, 0.02);
}

void FaceBeautification::curve(cv::Mat &image, int beta){
	int r, g, b;
	double s;
	int table[256];
	for(int i = 0; i < 256; i++){
		s = log(i * 1.0 / 255 * (beta - 1) + 1) / (log(beta));
		s *= 255;
		if(s < 0)
			s = 0;
		else if(s > 255)
			s = 255;
		table[i] = (int)s;
	}
	for(int row = 0; row < image.rows; row++){
		for(int col = 0; col < image.cols; col++){
			b = image.at<cv::Vec3b>(row, col).val[0];
			g = image.at<cv::Vec3b>(row, col).val[1];
			r = image.at<cv::Vec3b>(row, col).val[2];

			b = table[b];
			g = table[g];
			r = table[r];

			image.at<cv::Vec3b>(row, col).val[0] = b;
			image.at<cv::Vec3b>(row, col).val[1] = g;
			image.at<cv::Vec3b>(row, col).val[2] = r;
		}
	}
}

//void FaceBeautification::genFaceMask2(cv::Mat &image){
//	IplImage* img = NULL;  
//    IplImage* cutImg = NULL;  
//    CvMemStorage* storage = cvCreateMemStorage(0);  
//    //CvHaarClassifierCascade* cascade = (CvHaarClassifierCascade*)cvLoad("data.xml", 0, 0, 0);  
//    CvHaarClassifierCascade* cascade = (CvHaarClassifierCascade*)cvLoad("haarcascade_frontalface_alt2.xml", 0, 0, 0);  
//    CvSeq* faces;  
//    char save_path[100];  
//    char path[100];  
//    int j;  
//    int index = -1;  
//    for (int i = 5 ; i <= 100; i++)  
//    {  
//      
//        sprintf(path,"..\\20140622jiezhen\\%d.bmp", i);  
//        sprintf(save_path,"..\\GABCUT\\%d.bmp", i);  
//        img = cvLoadImage(path, 0);  
//      
//          
//        faces = cvHaarDetectObjects(img, cascade,  storage, 1.2, 2, 0, cvSize(25,25) );  
//        if (faces->total == 0)  
//            continue;  
//        cvSetImageROI(img, *((CvRect*)cvGetSeqElem( faces, 0)));   
//        cvSaveImage(save_path, img);      
//        cvResetImageROI(img);     
//          
//    }  
//}