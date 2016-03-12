#include "ColorTransfer.h"
#include<cmath>
#include "PatchMatch.h"
using namespace cv; 

void ColorTransfer::localVarianceTransfer(Mat &c1, Mat &c2){
	//计算局部方差
	Mat sV, tV, c3, c4;
	Mat srcImg_lab = c1;
	Mat targetImg_lab = c2;

	srcImg_lab.convertTo(srcImg_lab, CV_32FC3,1.0f/255.f);  
    targetImg_lab.convertTo(targetImg_lab, CV_32FC3, 1.0f/255.0f);  
    cvtColor(srcImg_lab, srcImg_lab, CV_BGR2Lab );  
    cvtColor(targetImg_lab, targetImg_lab, CV_BGR2Lab);  


	computeLocalVariances(srcImg_lab, 2, sV);
	computeLocalVariances(targetImg_lab, 2, tV);
	//imshow("sV", sV);
	//imshow("tV", tV);


	//扫面线方法

	//取种子点
	int width_offset = srcImg_lab.cols / 15;
	int height_offset = srcImg_lab.rows / 15;
	int recordRow[225];
	int recordCol[225];
	float recordValueL[225];
	float recordValueS[225];
	//cvtColor(c1, c3, CV_BGR2Lab );  
    //cvtColor(c2, c4, CV_BGR2Lab); 

	//imshow("c1",srcImg_lab);
	//imshow("c2",targetImg_lab);

	int num = 0;
	for(int row = 0; row < srcImg_lab.rows - height_offset; row += height_offset){
		for(int col = 0; col < srcImg_lab.cols - width_offset; col += width_offset){
			int crow = row + (rand() % height_offset);
			int ccol = col + (rand() % width_offset);
			recordValueL[num] = srcImg_lab.at<Vec3f>(crow, ccol).val[0];
			recordValueS[num] = sV.at<Vec3f>(crow, ccol).val[0];
			recordRow[num] = crow;
			recordCol[num] = ccol;
			num++;
		}
	}
	Mat targetImg_lab_copy;
	targetImg_lab.copyTo(targetImg_lab_copy);

	for(int row = 0; row < targetImg_lab.rows; row++){
		for(int col = 0; col < targetImg_lab.cols; col++){
			float td = targetImg_lab.at<Vec3f>(row, col).val[0];
			float tl = tV.at<Vec3f>(row, col).val[0];
			float min = 10000;
			int x = 0, y = 0;
			for(int i = 0; i < num; i++){
				int dif = abs(td - recordValueL[i]) + abs(tl - recordValueS[i]);
				if(dif < 0) dif *= -1;
				if(dif < min){
					min = dif;
					x = recordCol[i];
					y = recordRow[i];
				}
			}
			targetImg_lab_copy.at<Vec3f>(row, col) = srcImg_lab.at<Vec3f>(y, x);
			//targetImg_lab_copy.at<Vec3f>(row, col).val[2] = srcImg_lab.at<Vec3f>(y, x).val[2];
		}
	}

	Mat result_lab(targetImg_lab_copy.rows, targetImg_lab_copy.cols, CV_32FC3);  
	cvtColor(targetImg_lab_copy, result_lab, CV_Lab2BGR);  
	imshow("resVariance", result_lab);
}

void ColorTransfer::fractalTransfer(Mat &srcImg_32F, Mat &targetImg_32F, Mat &c1, Mat &c2){
	computeFractal(srcImg_lab, 4, srcD, srcL);
	computeFractal(targetImg_lab, 4, targetD, targetL);
	Mat c3, c4;
	float max = 0;
	for(int row = 0; row < srcImg_32F.rows; row++){
		for(int col = 0; col < srcImg_32F.cols; col++){
			srcImg_32F.at<Vec3f>(row, col).val[0] = srcD[row * srcImg_32F.cols + col];
			srcImg_32F.at<Vec3f>(row, col).val[1] = srcD[row * srcImg_32F.cols + col];
			srcImg_32F.at<Vec3f>(row, col).val[2] = srcD[row * srcImg_32F.cols + col];
		}
	}

	//for(int row = 0; row < srcImg_32F.rows; row++){
	//	for(int col = 0; col < srcImg_32F.cols; col++){
	//		if(srcImg_32F.at<Vec3f>(row, col).val[0] > max)
	//			max = srcImg_32F.at<Vec3f>(row, col).val[0];
	//	}
	//}
	//for(int row = 0; row < srcImg_32F.rows; row++){
	//	for(int col = 0; col < srcImg_32F.cols; col++){
	//		srcImg_32F.at<Vec3f>(row, col).val[0] /= max;
	//		srcImg_32F.at<Vec3f>(row, col).val[1] /= max;
	//		srcImg_32F.at<Vec3f>(row, col).val[2] /= max;
	//	}
	//}


	max = 0;
	for(int row = 0; row < targetImg_32F.rows; row++){
		for(int col = 0; col < targetImg_32F.cols; col++){
			targetImg_32F.at<Vec3f>(row, col).val[0] = targetD[row * targetImg_32F.cols + col];
			targetImg_32F.at<Vec3f>(row, col).val[1] = targetD[row * targetImg_32F.cols + col];
			targetImg_32F.at<Vec3f>(row, col).val[2] = targetD[row * targetImg_32F.cols + col];
		}
	}
	//for(int row = 0; row < targetImg_32F.rows; row++){
	//	for(int col = 0; col < targetImg_32F.cols; col++){
	//		if(targetImg_32F.at<Vec3f>(row, col).val[0] > max)
	//			max = targetImg_32F.at<Vec3f>(row, col).val[0];
	//	}
	//}
	//for(int row = 0; row < targetImg_32F.rows; row++){
	//	for(int col = 0; col < targetImg_32F.cols; col++){
	//		targetImg_32F.at<Vec3f>(row, col).val[0] /= max;
	//		targetImg_32F.at<Vec3f>(row, col).val[1] /= max;
	//		targetImg_32F.at<Vec3f>(row, col).val[2] /= max;
	//	}
	//}

	imshow("sD", srcImg_32F);
	imshow("tD", targetImg_32F);

	//Patchmatch
	//PatchMatch patchMatch;
	//BITMAP* ann = new BITMAP(targetImg_32F.cols, targetImg_32F.rows);
	//BITMAP annd(targetImg_32F.cols, targetImg_32F.rows);
	   //得到fractal信息
	Mat srcImg_char(c1.size(), CV_64F), targetImg_char(c2.size(), CV_64F); 
	c1.copyTo(srcImg_char);
	c2.copyTo(targetImg_char);
	for(int row = 0; row < targetImg_32F.rows; row++){
		for(int col = 0; col < targetImg_32F.cols; col++){
			int b = int(targetImg_32F.at<Vec3f>(row, col).val[0] * 255);
			//b = 0;
			targetImg_char.at<Vec3b>(row, col).val[0] = b;
			targetImg_char.at<Vec3b>(row, col).val[1] = b;//int(targetImg_lab.at<Vec3f>(row, col).val[0]);//b;//int(tV.at<Vec3f>(row, col).val[1] * 255);
			targetImg_char.at<Vec3b>(row, col).val[2] = int(targetImg_lab.at<Vec3f>(row, col).val[0]);
		}
	}

	for(int row = 0; row < srcImg_32F.rows; row++){
		for(int col = 0; col < srcImg_32F.cols; col++){
			int b = int(srcImg_32F.at<Vec3f>(row, col).val[0] * 255);
			//b = 0;
			srcImg_char.at<Vec3b>(row, col).val[0] = b;//int(srcImg_32F.at<Vec3f>(row, col).val[0] * 255);
			srcImg_char.at<Vec3b>(row, col).val[1] = b;//int(srcImg_lab.at<Vec3f>(row, col).val[0]);//b;//int(sV.at<Vec3f>(row, col).val[1] * 255);
			srcImg_char.at<Vec3b>(row, col).val[2] = int(srcImg_lab.at<Vec3f>(row, col).val[0]);
		}
	}


	// 1 Patchmatch方法

	// 1.1用fractal得到patchmatch种子点
	//int width_offset = targetImg_char.cols / 10;
	//int height_offset = targetImg_char.rows / 10;
	//for(int row = 0; row < targetImg_char.rows; row += height_offset){
	//	for(int col = 0; col < targetImg_char.cols; col += width_offset){
	//		int tl = targetImg_char.at<Vec3b>(row, col).val[0];
	//		int x = 0, y = 0;
	//		int dif = 1000;
	//		for(int row1 = 0; row1 < srcImg_32F.rows - 7; row1++){
	//			for(int col1 = 0; col1 < srcImg_32F.cols - 7; col1++){
	//				int b = int(srcImg_32F.at<Vec3f>(row1, col1).val[0] * 255);
	//				int m = b - tl;
	//				if(m < 0) m*= -1;
	//				if(m < dif){
	//					dif = m;
	//					x = col1;
	//					y = row1;
	//				}
	//			}
	//		}

	//		(*ann)[row][col] = XY_TO_INT(x, y);
	//	}
	//}
	//patchMatch.patchmatch(targetImg_char, srcImg_char, ann, annd);////////////////////////////////////////////////////////////////////


	////1.2直接用灰度做patchmatch
	//cvtColor(c1, c3, CV_BGR2Lab );  
 //   cvtColor(c2, c4, CV_BGR2Lab);  
	//for(int row = 0; row < targetImg_32F.rows; row++){
	//	for(int col = 0; col < targetImg_32F.cols; col++){
	//		int b = c4.at<Vec3b>(row, col).val[0];
	//		c4.at<Vec3b>(row, col).val[1] = b;//int(targetImg_lab.at<Vec3f>(row, col).val[0]);//b;//int(tV.at<Vec3f>(row, col).val[1] * 255);
	//		c4.at<Vec3b>(row, col).val[2] = b;
	//	}
	//}

	//for(int row = 0; row < srcImg_32F.rows; row++){
	//	for(int col = 0; col < srcImg_32F.cols; col++){
	//		int b = c3.at<Vec3b>(row, col).val[0];
	//		c3.at<Vec3b>(row, col).val[1] = b;//int(srcImg_lab.at<Vec3f>(row, col).val[0]);//b;//int(sV.at<Vec3f>(row, col).val[1] * 255);
	//		c3.at<Vec3b>(row, col).val[2] = b;
	//	}
	//}

	//imshow("c3", c3);
	//imshow("c4", c4);

	//patchMatch.patchmatch(c4, c3, ann, annd);////////////////////////////////////////////////////////////////////
	////// 
	////patchMatch.vote(c4, c2, ann);
	////imshow("c5", c4);

	//////////对灰度图patchmatch
	////得到patchmatch最终映射
	//Mat targetImg_lab_copy;
	//targetImg_lab.copyTo(targetImg_lab_copy);

	//for(int i = 0; i < targetImg_lab_copy.rows - 7 + 1; i++){
	//	for(int j = 0; j < targetImg_lab_copy.cols - 7 + 1; j++){
	//		int v = (*ann)[i][j];
	//		int x = INT_TO_X(v);
	//		int y = INT_TO_Y(v);
	//		//targetImg_lab_copy.at<Vec3f>(i, j).val[0] = srcImg_lab.at<Vec3f>(y, x).val[0];
	//		targetImg_lab_copy.at<Vec3f>(i, j).val[1] = srcImg_lab.at<Vec3f>(y, x).val[1];
	//		targetImg_lab_copy.at<Vec3f>(i, j).val[2] = srcImg_lab.at<Vec3f>(y, x).val[2];
	//	}
	//}
	//Mat result_lab(targetImg_lab_copy.rows, targetImg_lab_copy.cols, CV_32FC3);  
	//cvtColor(targetImg_lab_copy, result_lab, CV_Lab2BGR);  
	//imshow("resFractalPatchMatch", result_lab);






	//  2 扫面线方法
	//得到原图种子点
	int width_offset = srcImg_char.cols / 15;
	int height_offset = srcImg_char.rows / 15;
	int recordRow[225];
	int recordCol[225];
	int recordValue[225];

	cvtColor(c1, c3, CV_BGR2Lab);  
    cvtColor(c2, c4, CV_BGR2Lab); 
	int num = 0;
	for(int row = 0; row < srcImg_char.rows - height_offset; row += height_offset){
		for(int col = 0; col < srcImg_char.cols - width_offset; col += width_offset){
			int crow = row + (rand() % height_offset);
			int ccol = col + (rand() % width_offset);
			int td = srcImg_char.at<Vec3b>(crow, ccol).val[0];
			int tl = c3.at<Vec3b>(crow, ccol).val[0];
			recordValue[num] = td + tl;
			recordRow[num] = crow;
			recordCol[num] = ccol;
			num++;
		}
	}
	Mat targetImg_lab_copy;
	targetImg_lab.copyTo(targetImg_lab_copy);

	for(int row = 0; row < targetImg_char.rows; row++){
		for(int col = 0; col < targetImg_char.cols; col++){
			int td = targetImg_char.at<Vec3b>(row, col).val[0];
			int tl = c4.at<Vec3b>(row, col).val[0];
			int re = td + tl;
			int min = 10000;
			int x = 0, y = 0;
			for(int i = 0; i < num; i++){
				int dif = re - recordValue[i];
				if(dif < 0) dif *= -1;
				if(dif < min){
					min = dif;
					x = recordCol[i];
					y = recordRow[i];
				}
			}
			targetImg_lab_copy.at<Vec3f>(row, col).val[1] = srcImg_lab.at<Vec3f>(y, x).val[1];
			targetImg_lab_copy.at<Vec3f>(row, col).val[2] = srcImg_lab.at<Vec3f>(y, x).val[2];
		}
	}

	Mat result_lab(targetImg_lab_copy.rows, targetImg_lab_copy.cols, CV_32FC3);  
	cvtColor(targetImg_lab_copy, result_lab, CV_Lab2BGR);  
	imshow("resFractal", result_lab);
}
void ColorTransfer::initialize()  
{  
    Mat srcImg_32F, targetImg_32F;  

	Mat c1, c2, c3, c4; 
	srcImg.copyTo(c1);
	//cvtColor(c, srcImg_char, CV_BGR2Lab ); 
	targetImg.copyTo(c2);
	//cvtColor(c, targetImg_char, CV_BGR2Lab ); 


    srcImg.convertTo(srcImg_32F, CV_32FC3,1.0f/255.f);  
    targetImg.convertTo(targetImg_32F, CV_32FC3, 1.0f/255.0f);  
    cvtColor(srcImg_32F, srcImg_lab, CV_BGR2Lab );  
    cvtColor(targetImg_32F, targetImg_lab, CV_BGR2Lab);  


    //if(srcImg_lab.depth() == CV_32FC3) cout << "Yes" << endl;  
    cout << srcImg_lab.depth() << endl;  
    cout << srcImg.depth() << endl;  
    computeMeans();  
    computeVariances();  
	
	//fractalTransfer(srcImg_32F, targetImg_32F, c1, c2);
	//localVarianceTransfer(c1, c2);
	Mat sV, tV;
	computeLocalVariances(srcImg_lab, 4, sV);
	computeLocalVariances(targetImg_lab, 4, tV);
	//imshow("sV", sV);
	//imshow("tV", tV);
}  
void ColorTransfer::computeMeans(){  
    int srcPixels = srcImg.rows * srcImg.cols;  
    int targetPixels = targetImg.rows * targetImg.cols;  
    //computing the mean of source image in lab space  
    float sum[3] ={ 0 };  
    for (int row = 0; row < srcImg.rows; row++)  
    {  
        for (int col = 0; col < srcImg.cols; col++)  
        {  
            //Point pt(col, row);  
            Vec3f color = srcImg_lab.at<Vec3f>(row, col);  
            for(int layer = 0; layer < 3; layer++)  
                sum[layer] += color[layer];               
        }  
    }  
    for (int i = 0; i < 3; i++)  
        srcMeans[i] = sum[i] / srcPixels;  
  
    //computing the mean of target image int lab color space  
    for(int i = 0; i < 3; i++)  
        sum[i] = 0;  
    for (int row = 0; row < targetImg.rows; row++)  
    {  
        for (int col = 0; col < targetImg.cols; col++)  
        {  
            //Point pt(col, row);  
            Vec3f color = targetImg_lab.at<Vec3f>(row, col);  
            for(int layer = 0; layer < 3; layer++)  
                sum[layer] += color[layer];               
        }  
    }  
    for (int i = 0; i < 3; i++)  
        targetMeans[i] = sum[i] / targetPixels;  
}//end function compuetMeans  
void ColorTransfer::computeVariances(){  
    int srcPixels =  srcImg_lab.cols * srcImg_lab.rows;  
    int targetPixels = targetImg_lab.cols * targetImg_lab.rows;  
    //computing the variance of source image   
    float sum_variance[3] ={0.f};  
    for (int y = 0; y < srcImg_lab.rows; y++)  
    {  
        for (int x = 0; x < srcImg_lab.cols; x++)  
        {  
            Vec3f color = srcImg_lab.at<Vec3f>(y,x);  
            sum_variance[0] += (color[0] - srcMeans[0])*(color[0] - srcMeans[0]);  
            sum_variance[1] += (color[1] - srcMeans[1])*(color[1] - srcMeans[1]);  
            sum_variance[2] += (color[2] - srcMeans[2])*(color[2] - srcMeans[2]);  
        }  
    }  
    srcVariances[0] = sqrt( sum_variance[0] / srcPixels );  
    srcVariances[1] = sqrt( sum_variance[1] / srcPixels );  
    srcVariances[2] = sqrt( sum_variance[2] / srcPixels );        
          
    //computing the variance of target image   
    for(int i = 0; i < 3; i++)  
        sum_variance[i] = 0.f;  
  
    for (int y = 0; y < targetImg_lab.rows; y++)  
    {  
        for (int x = 0; x < targetImg_lab.cols; x++)  
        {  
            Vec3f color = targetImg_lab.at<Vec3f>(y,x);  
            sum_variance[0] += (color[0] - targetMeans[0])*(color[0] - targetMeans[0]);  
            sum_variance[1] += (color[1] - targetMeans[1])*(color[1] - targetMeans[1]);  
            sum_variance[2] += (color[2] - targetMeans[2])*(color[2] - targetMeans[2]);  
        }  
    }  
    targetVariances[0] = sqrt( sum_variance[0] / targetPixels );  
    targetVariances[1] = sqrt( sum_variance[1] / targetPixels );  
    targetVariances[2] = sqrt( sum_variance[2] / targetPixels );  
  
  
} 

void ColorTransfer::computeLocalVariances(Mat &image, int radius, Mat &result){
	image.copyTo(result);
	int rows = image.rows;
	int cols = image.cols;
	for(int row = 0; row < rows; row++){
		for(int col = 0; col < cols; col++){
		
			float sum = 0;
			for(int i = -radius; i <= radius; i++){
				for(int j = -radius; j <= radius; j++){
					int cr = row + i;
					int cc = col + j;
					if(cr < 0){
						cr *= -1;
					}
					if(cc < 0){
						cc *= -1;
					}
					if(cr >= rows){
						cr = 2 * rows - 2 - cr;
					}
					if(cc >= cols){
						cc = 2 * cols - 2 - cc;
					}
					sum += image.at<Vec3f>(cr, cc).val[0];
				}
			}
			float mean = sum / ((2 * radius + 1) * (2 * radius + 1));
			sum = 0;
			for(int i = -radius; i <= radius; i++){
				for(int j = -radius; j <= radius; j++){
					int cr = row + i;
					int cc = col + j;
					if(cr < 0){
						cr *= -1;
					}
					if(cc < 0){
						cc *= -1;
					}
					if(cr >= rows){
						cr = 2 * rows - 2 - cr;
					}
					if(cc >= cols){
						cc = 2 * cols - 2 - cc;
					}
					sum += (image.at<Vec3f>(cr, cc).val[0] - mean) * (image.at<Vec3f>(cr, cc).val[0] - mean);
				}
			}
			sum /= ((2 * radius + 1) * (2 * radius + 1));

			result.at<Vec3f>(row, col).val[0] = sum;
			result.at<Vec3f>(row, col).val[1] = sum;
			result.at<Vec3f>(row, col).val[2] = sum;
			
				
		}
	}
	float max = 0;
	for(int row = 0; row < rows; row++){
		for(int col = 0; col < cols; col++){
			if(result.at<Vec3f>(row, col).val[0] > max)
				max = result.at<Vec3f>(row, col).val[0];
		}
	}
	for(int row = 0; row < rows; row++){
		for(int col = 0; col < cols; col++){
			result.at<Vec3f>(row, col).val[0] /= max;
			result.at<Vec3f>(row, col).val[1] /= max;
			result.at<Vec3f>(row, col).val[2] /= max;
		}
	}
}
void ColorTransfer::computeFractal(Mat &image, int radius, float* &D, float* &L){
	if(D != NULL)
		delete D;
	if(L != NULL)
		delete L;

	L = new float[image.rows * image.cols];
	D = new float[image.rows * image.cols];
	float R[] = {1, 2, 3, 4};
	float **I = new float*[4];
	for(int i = 0; i < 4; i++){
		I[i] = new float[image.rows * image.cols];
	}
	int rows = image.rows;
	int cols = image.cols;
	for(int row = 0; row < rows; row++){
		for(int col = 0; col < cols; col++){
			L[row * cols + col] = image.at<Vec3f>(row, col).val[0];
		}
	}

	//compute fractal
	
	
	for(int row = 0; row < rows; row++){
		for(int col = 0; col < cols; col++){
			
			for(int r = 1; r < 5; r++){
				float sum = 0;
				for(int i = -r; i <= r; i++){
					for(int j = -r; j <= r; j++){
						int cr = row + i;
						int cc = col + j;
						if(cr < 0){
							cr *= -1;
						}
						if(cc < 0){
							cc *= -1;
						}
						if(cr >= rows){
							cr = 2 * rows - 2 - cr;
						}
						if(cc >= cols){
							cc = 2 * cols - 2 - cc;
						}
						sum += L[cr * cols + cc];
					}
				}
				if(sum > 0)
					I[r-1][row * cols + col] = log(sum);
				else
					I[r-1][row * cols + col] = 0;
			}
				
		}
	}
	for(int row = 0; row < rows; row++){
		for(int col = 0; col < cols; col++){
			float Imean = 0.0;
			float Rmean = 0.0;
			float RR = 0.0;
			float IR = 0.0;
			for(int i = 0; i < 4; i++){
				Imean += I[i][row * cols + col];
				Rmean += R[i];
				IR += I[i][row * cols + col] * R[i];
				RR += R[i] * R[i];
			}
			Imean /= 4;
			Rmean /= 4;
	
			float cD = (IR - 4 * Imean * Rmean) / (RR - 4 * Rmean * Rmean);
			if(cD <0) cD = 0;
			D[row * cols + col] = cD;
		}
	}
	//delete L;
	for(int i = 0; i < 4; i++){
		delete I[i];
	}
	delete I;
}
void ColorTransfer::solve()  
{  
    initialize();  
    //constructing the final value in very pixel and store the value in result matrix  
    int width = srcImg.cols;  
    int height = srcImg.rows;  
    //Mat result(height, width, CV_32FC3);  
    Mat result_lab(height, width, CV_32FC3);  
    float deta_rate[3];  
    for (int k = 0; k < 3; k++)  
    {  
        deta_rate[k] = targetVariances[k] / srcVariances[k];  
    }  
    for (int y = 0; y < height; y++ )  
    {  
        for (int x = 0; x < width; x++)  
        {  
            Vec3f color = srcImg_lab.at<Vec3f>(y,x);  
            Vec3f value;   
            for(int channel = 0; channel < 3; channel++)  
                value[channel] = deta_rate[channel]*(color[channel] - srcMeans[channel]) + targetMeans[channel];  
            result_lab.at<Vec3f>(y,x) = value;   
  
        }  
    }  
    //construct the final image  
    cvtColor(result_lab, result, CV_Lab2BGR);  
}  