#include "Beeps.h"
#include <cmath>
Beeps::Beeps(){}
void Beeps::beeps(cv::Mat &image, int iter, double photometricStandardDeviationval, double spatialDecayval){
	int row = image.rows;
	int col = image.cols;
	photometricStandardDeviation = photometricStandardDeviationval;
	spatialDecay = spatialDecayval;
	for(int i = 0; i < iter; i++){
		cv::Mat duplicateImg(image.size(), CV_64F);
		image.copyTo(duplicateImg);
		beepsHorizontalVertical(duplicateImg);
		beepsVerticalHorizontal(image);
		
		for(int row = 0; row < image.rows; row++){
			for(int col = 0; col < image.cols; col++){
				image.at<cv::Vec3b>(row, col) = (image.at<cv::Vec3b>(row, col) + duplicateImg.at<cv::Vec3b>(row, col)) / 2; 
			}
		}
	}
}

void Beeps::beepsVerticalHorizontal(cv::Mat &image){
	cv::Mat p(image.size(), CV_64F);
	cv::Mat r(image.size(), CV_64F);
	cv::Mat g(image.size(), CV_64F);
	//img = image.t();
	g = image.t();

	g.copyTo(p);
	g.copyTo(r);

	progressive(p);
	gain(g);
	regressive(r);

	for(int row = 0; row < g.rows; row++){
		for(int col = 0; col < g.cols; col++){
			r.at<cv::Vec3b>(row, col) += p.at<cv::Vec3b>(row, col) - g.at<cv::Vec3b>(row, col); 
		}
	}

	g = r.t();
	g.copyTo(p);
	g.copyTo(r);

	progressive(p);
	gain(g);
	regressive(r);

	for(int row = 0; row < g.rows; row++){
		for(int col = 0; col < g.cols; col++){
			image.at<cv::Vec3b>(row, col) = p.at<cv::Vec3b>(row, col) - g.at<cv::Vec3b>(row, col) + r.at<cv::Vec3b>(row, col); 
		}
	}

}

void Beeps::beepsHorizontalVertical(cv::Mat &image){
	cv::Mat p(image.size(), CV_64F);
	cv::Mat r(image.size(), CV_64F);
	cv::Mat g(image.size(), CV_64F);

	image.copyTo(g);
	g.copyTo(p);
	g.copyTo(r);

	progressive(p);
	gain(g);
	regressive(r);

	for(int row = 0; row < g.rows; row++){
		for(int col = 0; col < g.cols; col++){
			r.at<cv::Vec3b>(row, col) += p.at<cv::Vec3b>(row, col) - g.at<cv::Vec3b>(row, col); 
		}
	}

	g = r.t();
	g.copyTo(p);
	g.copyTo(r);

	progressive(p);
	gain(g);
	regressive(r);

	for(int row = 0; row < g.rows; row++){
		for(int col = 0; col < g.cols; col++){
			r.at<cv::Vec3b>(row, col) += p.at<cv::Vec3b>(row, col) - g.at<cv::Vec3b>(row, col); 
		}
	}
	image = r.t();
}

void Beeps::progressive(cv::Mat &image){
	double mur, mug, mub;
	double c = -0.5 / (photometricStandardDeviation * photometricStandardDeviation);
	double spatialContraDecay = 1 - spatialDecay;
	double rho = spatialContraDecay + 1;

	double result;
	for(int row = 0; row < image.rows; row++){
		for(int col = 0; col < image.cols; col++){
			if(col == 0){
				image.at<cv::Vec3b>(row, col).val[0] = (char)(image.at<cv::Vec3b>(row, col).val[0] / rho);
				image.at<cv::Vec3b>(row, col).val[1] = (char)(image.at<cv::Vec3b>(row, col).val[1] / rho);
				image.at<cv::Vec3b>(row, col).val[2] = (char)(image.at<cv::Vec3b>(row, col).val[2] / rho);
			}
			else{
				mub = image.at<cv::Vec3b>(row, col).val[0] - rho * image.at<cv::Vec3b>(row, col - 1).val[0];
				mub = spatialContraDecay * exp(c * mub * mub);
				result = image.at<cv::Vec3b>(row, col - 1).val[0] * mub + image.at<cv::Vec3b>(row, col).val[0] * (1 - mub) / rho;
				image.at<cv::Vec3b>(row, col).val[0] = (char)result;

				mug = image.at<cv::Vec3b>(row, col).val[1] - rho * image.at<cv::Vec3b>(row, col - 1).val[1];
				mug = spatialContraDecay * exp(c * mug * mug);
				result = image.at<cv::Vec3b>(row, col - 1).val[1] * mug + image.at<cv::Vec3b>(row, col).val[1] * (1 - mug) / rho;
				image.at<cv::Vec3b>(row, col).val[1] = (char)result;

				mur = image.at<cv::Vec3b>(row, col).val[2] - rho * image.at<cv::Vec3b>(row, col - 1).val[2];
				mur = spatialContraDecay * exp(c * mur * mur);
				result = image.at<cv::Vec3b>(row, col - 1).val[2] * mur + image.at<cv::Vec3b>(row, col).val[2] * (1 - mur) / rho;
				image.at<cv::Vec3b>(row, col).val[2] = (char)result;
			}
		}
	}
}

void Beeps::regressive(cv::Mat &image){
	double mur, mug, mub;
	double c = -0.5 / (photometricStandardDeviation * photometricStandardDeviation);
	double spatialContraDecay = 1 - spatialDecay;
	double rho = spatialContraDecay + 1;

	double result;
	for(int row = 0; row < image.rows; row++){
		for(int col = image.cols - 1; col >= 0; col--){
			if(col == image.cols - 1){
				image.at<cv::Vec3b>(row, col).val[0] = (char)(image.at<cv::Vec3b>(row, col).val[0] / rho);
				image.at<cv::Vec3b>(row, col).val[1] = (char)(image.at<cv::Vec3b>(row, col).val[1] / rho);
				image.at<cv::Vec3b>(row, col).val[2] = (char)(image.at<cv::Vec3b>(row, col).val[2] / rho);
			}
			else{
				mub = image.at<cv::Vec3b>(row, col).val[0] - rho * image.at<cv::Vec3b>(row, col + 1).val[0];
				mub = spatialContraDecay * exp(c * mub * mub);
				result = image.at<cv::Vec3b>(row, col + 1).val[0] * mub + image.at<cv::Vec3b>(row, col).val[0] * (1 - mub) / rho;
				image.at<cv::Vec3b>(row, col).val[0] = (char)result;

				mug = image.at<cv::Vec3b>(row, col).val[1] - rho * image.at<cv::Vec3b>(row, col + 1).val[1];
				mug = spatialContraDecay * exp(c * mug * mug);
				result = image.at<cv::Vec3b>(row, col + 1).val[1] * mug + image.at<cv::Vec3b>(row, col).val[1] * (1 - mug) / rho;
				image.at<cv::Vec3b>(row, col).val[1] = (char)result;

				mur = image.at<cv::Vec3b>(row, col).val[2] - rho * image.at<cv::Vec3b>(row, col + 1).val[2];
				mur = spatialContraDecay * exp(c * mur * mur);
				result = image.at<cv::Vec3b>(row, col + 1).val[2] * mur + image.at<cv::Vec3b>(row, col).val[2] * (1 - mur) / rho;
				image.at<cv::Vec3b>(row, col).val[2] = (char)result;
			}
		}
	}
}

void Beeps::gain(cv::Mat &image){
	double spatialContraDecay = 1 - spatialDecay;
	double mu = (1 - spatialContraDecay) * (1 + spatialContraDecay);

	for(int row = 0; row < image.rows; row++){
		for(int col = 0; col < image.cols; col++){
			image.at<cv::Vec3b>(row, col).val[0] = (char)(image.at<cv::Vec3b>(row, col).val[0] * mu);
			image.at<cv::Vec3b>(row, col).val[1] = (char)(image.at<cv::Vec3b>(row, col).val[1] * mu);
			image.at<cv::Vec3b>(row, col).val[2] = (char)(image.at<cv::Vec3b>(row, col).val[2] * mu);
		}
	}
}

void Beeps::beepsVerticalHorizontal(double *data, int width, int height){
	double *g = new double[width * height];

	int m = 0;
	for (int k2 = 0; (k2 < height); k2++) {
		int n = k2;
		for (int k1 = 0; (k1 < width); k1++) {
			g[n] = (double)data[m++];
			n += height;
		}
	}
	//dataCopy(data, g, width * height);

	double *p = new double[width * height];
	double *r = new double[width * height];

	dataCopy(g, p, width * height);
	dataCopy(g, r, width * height);

	for (int k1 = 0; (k1 < width); k1++) {
		progressive(p, k1 * height, height);
		gain(g, k1 * height, height);
		regressive(r, k1 * height, height);
	}

	for (int k = 0, K = width * height; (k < K); k++) {
		r[k] += p[k] - g[k];
	}

	m = 0;
	for (int k1 = 0; (k1 < width); k1++) {
		int n = k1;
		for (int k2 = 0; (k2 < height); k2++) {
			g[n] = r[m++];
			n += width;
		}
	}

	dataCopy(g, p, width * height);
	dataCopy(g, r, width * height);

	for (int k2 = 0; (k2 < height); k2++) {
		progressive(p, k2 * width, width);
		gain(g, k2 * width, width);
		regressive(r, k2 * width, width);
	}

	for (int k = 0, K = width * height; (k < K); k++) {
		data[k] = (float)(p[k] - g[k] + r[k]);
	}
}
void Beeps::beepsHorizontalVertical(double *data, int width, int height){
	double *g = new double[width * height];
	dataCopy(data, g, width * height);

	double *p = new double[width * height];
	double *r = new double[width * height];

	dataCopy(g, p, width * height);
	dataCopy(g, r, width * height);

	for (int k2 = 0; (k2 < height); k2++) {
		progressive(p, k2 * width, width);
		gain(g, k2 * width, width);
		regressive(r, k2 * width, width);
	}
	for (int k = 0, K = width * height; (k < K); k++) {
		r[k] += p[k] - g[k];
	}

	int m = 0;
	for (int k2 = 0; (k2 < height); k2++) {
		int n = k2;
		for (int k1 = 0; (k1 < width); k1++) {
			g[n] = r[m++];
			n += height;
		}
	}

	dataCopy(g, p, width * height);
	dataCopy(g, r, width * height);

	for (int k1 = 0; (k1 < width); k1++) {
		progressive(p, k1 * height, height);
		gain(g, k1 * height, height);
		regressive(r, k1 * height, height);
	}
	for (int k = 0, K = width * height; (k < K); k++) {
		r[k] += p[k] - g[k];
	}

	m = 0;
	for (int k1 = 0; (k1 < width); k1++) {
		int n = k1;
		for (int k2 = 0; (k2 < height); k2++) {
			data[n] = (float)r[m++];
			n += width;
		}
	}
}
void Beeps::progressive(double *data, int startIndex, int length){
	double c = -0.5 / (photometricStandardDeviation * photometricStandardDeviation);
	double spatialContraDecay = 1 - spatialDecay;
	double rho = spatialContraDecay + 1;

	double mu = 0.0;
	data[startIndex] /= rho;

	for (int k = startIndex + 1, K = startIndex + length;(k < K); k++) {
		mu = data[k] - rho * data[k - 1];
		mu = spatialContraDecay * exp(c * mu * mu);
		data[k] = data[k - 1] * mu + data[k] * (1.0 - mu) / rho;
	}
}
void Beeps::regressive(double *data, int startIndex, int length){
	double c = -0.5 / (photometricStandardDeviation * photometricStandardDeviation);
	double spatialContraDecay = 1 - spatialDecay;
	double rho = spatialContraDecay + 1;

	double mu = 0.0;
	data[startIndex + length - 1] /= rho;

	for (int k = startIndex + length - 2; (startIndex <= k); k--) {
		mu = data[k] - rho * data[k + 1];
		mu = spatialContraDecay * exp(c * mu * mu);
		data[k] = data[k + 1] * mu + data[k] * (1.0 - mu) / rho;
	}
}
void Beeps::gain(double *data, int startIndex, int length){
	double spatialContraDecay = 1 - spatialDecay;
	double mu = (1 - spatialContraDecay) * (1 + spatialContraDecay);

	for (int k = startIndex, K = startIndex + length; (k < K); k++) {
		data[k] *= mu;
	}
}

void Beeps::dataCopy(double *data, double *target, int length){
	for(int i = 0; i < length; i++)
		target[i] = data[i];
}

void Beeps::beeps2(cv::Mat &image, int iter, double photometricStandardDeviationval, double spatialDecayval){
	int rows = image.rows;
	int cols = image.cols;
	photometricStandardDeviation = photometricStandardDeviationval;
	spatialDecay = spatialDecayval;


	double *dataR = new double[rows * cols];
	double *dataG = new double[rows * cols];
	double *dataB = new double[rows * cols];
	double *dataDuplicateR = new double[rows * cols];
	double *dataDuplicateG = new double[rows * cols];
	double *dataDuplicateB = new double[rows * cols];
//#define length  550 * 500
//	double dataR[length];
//	double dataG[length];
//	double dataB[length];
//	double dataDuplicateR[length];
//	double dataDuplicateG[length];
//	double dataDuplicateB[length];
	int k = 0;
	for(int row = 0; row < rows; row++){
		for(int col = 0; col < cols; col++){
			dataB[k] = image.at<cv::Vec3b>(row, col).val[0];
			dataG[k] = image.at<cv::Vec3b>(row, col).val[1];
			dataR[k] = image.at<cv::Vec3b>(row, col).val[2];
			k++;
		}
	}
	
	for(int i = 0; i < iter; i++){
		dataCopy(dataB, dataDuplicateB, rows * cols);
		dataCopy(dataG, dataDuplicateG, rows * cols);
		dataCopy(dataR, dataDuplicateR, rows * cols);

		beepsHorizontalVertical(dataDuplicateB, cols, rows);
		beepsVerticalHorizontal(dataB, cols, rows);

		beepsHorizontalVertical(dataDuplicateG, cols, rows);
		beepsVerticalHorizontal(dataG, cols, rows);

		beepsHorizontalVertical(dataDuplicateR, cols, rows);
		beepsVerticalHorizontal(dataR, cols, rows);
		
		for(int i = 0; i < cols * rows; i++){
			dataB[i] = (dataB[i] + dataDuplicateB[i]) * 0.5;
			dataG[i] = (dataG[i] + dataDuplicateG[i]) * 0.5;
			dataR[i] = (dataR[i] + dataDuplicateR[i]) * 0.5;
		}
	}

	k = 0;
	int lower = 0, upper = 255;
	for(int row = 0; row < rows; row++){
		for(int col = 0; col < cols; col++){
			if(dataB[k] > upper) 
				dataB[k] = upper;
			else if(dataB[k] < lower)
				dataB[k] = lower;

			if(dataG[k] > upper) 
				dataG[k] = upper;
			else if(dataG[k] < lower)
				dataG[k] = lower;

			if(dataR[k] > upper) 
				dataR[k] = upper;
			else if(dataR[k] < lower)
				dataR[k] = lower;
			image.at<cv::Vec3b>(row, col).val[0] = (char)(dataB[k]);
			image.at<cv::Vec3b>(row, col).val[1] = (char)(dataG[k]);
			image.at<cv::Vec3b>(row, col).val[2] = (char)(dataR[k]);
			k++;
		}
	}
}