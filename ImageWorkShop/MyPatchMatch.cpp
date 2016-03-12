#include "MyPatchMatch.h"
#include <fstream>
using namespace std;
#define HOLE 0
#define SOURCE 100
#define OTHER 255
#define OFFSET_DATA_TYPE cv::Vec3w
#define OFFSET_MAT_TYPE CV_16SC3

//#define OFFSET_DATA_TYPE cv::Vec3b
//#define OFFSET_MAT_TYPE CV_8UC3
static int MaxValue = 65535;

MyPatchMatch::MyPatchMatch(){
	oldTopLeftX = 0;
	oldTopLeftY = 0;
	newTopLeftX = 0;
	newTopLeftY = 0;
	numberOfScales = 3;
	numberOfIterationPerScale = 5;
	ImageCopyRadius = 10;
	ImageCopyMode = ADDTARGET;
	ImageCopyMask = NULL;
	imageRows = 0;
	imageCols = 0;
	selectImageCopyPatch = false;
	mouseDownX = 0;
	mouseDownY = 0;
	ImageCopyMoveStart = false;
	interection = NO;
	//mouseDown = false;
	ImageInpaintMode = INPAINT_NO;
	ImageInpaintMask = NULL;
	InpaintRestrictMask = NULL;
	patch_radius = 2;
	ImageInpaintRestrictRadius = 4;
	ImageInpaintRadius = 4;
	initialSimilarity();
	erodeRadius = 0;
	srand((unsigned)time(NULL));
	initOpencl();
	mode = GPU_JUMPFLOODING;
}
MyPatchMatch::~MyPatchMatch(){
	deleteSimilarity();
	destroyOpencl();
}

/////////////////////////////////////////copy//////////////////////////////////////////////////////////
void MyPatchMatch::imageImageCopy(Mat& sourceImageRGB, Mat targetImageRGB){
	Mat sourceImage, targetImage, newOffsetMap;

	sourceImageRGB.convertTo(sourceImage,CV_32FC3,1/255.f);
	cvtColor(sourceImage,sourceImage,CV_RGB2Lab);
	targetImageRGB.convertTo(targetImage,CV_32FC3,1/255.f);
	cvtColor(targetImage,targetImage,CV_RGB2Lab);

	Mat offsetMap(targetImage.rows, targetImage.cols, CV_32FC3);
	initialOffset(offsetMap, true);
	//debugShowOffset("initial", offsetMap);

	Size orgSize = Size(targetImage.size());
	Mat tempTarget = targetImage.clone();

#pragma region //pyrdown
	for(int i = 0; i < numberOfScales; i++){
		pyrDown(tempTarget, tempTarget);
		newOffsetMap = Mat(tempTarget.rows,tempTarget.cols, offsetMap.type());
		for(int row = 0; row < newOffsetMap.rows; row++){
			int b = row * 2;
			if (b >= offsetMap.rows - 1)
				b = offsetMap.rows - 2;
			for (int col = 0; col < newOffsetMap.cols ;col++)
			{
				int a = col * 2;
				if (a >= offsetMap.cols - 1)
					a = offsetMap.cols - 2;


				newOffsetMap.at<Vec3f>(row, col).val[0] = offsetMap.at<Vec3f>(b,a).val[0] / 2;
				newOffsetMap.at<Vec3f>(row, col).val[1] = offsetMap.at<Vec3f>(b,a).val[1] / 2;

				if ((offsetMap.at<Vec3f>(b, a).val[2] == 255) || (offsetMap.at<Vec3f>(b + 1, a).val[2] == 255)
					|| (offsetMap.at<Vec3f>(b, a + 1).val[2] == 255) || (offsetMap.at<Vec3f>(b + 1 , a + 1).val[2] == 255))
				{
					if (offsetMap.at<Vec3f>(b + 1, a).val[2] == 255)
					{
						newOffsetMap.at<Vec3f>(row, col).val[0] = offsetMap.at<Vec3f>(b + 1, a).val[0] / 2;
						newOffsetMap.at<Vec3f>(row, col).val[1] = offsetMap.at<Vec3f>(b + 1, a).val[1] / 2;
					}
					else if (offsetMap.at<Vec3f>(b, a + 1).val[2] == 255)
					{
						newOffsetMap.at<Vec3f>(row, col).val[0] = offsetMap.at<Vec3f>(b, a + 1).val[0] / 2;
						newOffsetMap.at<Vec3f>(row, col).val[1] = offsetMap.at<Vec3f>(b, a + 1).val[1] / 2;
					}
					else if (offsetMap.at<Vec3f>(b + 1, a + 1).val[2] == 255)
					{
						newOffsetMap.at<Vec3f>(row, col).val[0] = offsetMap.at<Vec3f>(b + 1, a + 1).val[0] / 2;
						newOffsetMap.at<Vec3f>(row, col).val[1] = offsetMap.at<Vec3f>(b + 1, a + 1).val[1] / 2;
					}
					newOffsetMap.at<Vec3f>(row, col).val[2] = 255;
				}
				else
					newOffsetMap.at<Vec3f>(row, col).val[2] = 0;
			}
		}
		offsetMap = newOffsetMap.clone();
	}
#pragma endregion


	Mat tempSource = sourceImage.clone();
	scale(tempSource, numberOfScales);

	Mat tempTargetFiner;
	tempTarget.convertTo(tempTargetFiner, CV_32FC3, 1);

#pragma region //pyrup
	for(int i = numberOfScales; i > 0; i--){
		w = i * 50;
		if(i == numberOfScales){
			for (int k = 0; k < numberOfIterationPerScale; k++){
				//propagation
				propagation(tempSource, tempTargetFiner, offsetMap, (k)%2);

				//random  Search
				randomSearch(tempSource, tempTargetFiner, offsetMap);
			}
		}
		else{
			//Mat newOffsetMapFiner(offsetMap.rows, offsetMap.cols, offsetMap.type());
			//initialOffset(newOffsetMapFiner, false);
			//std::vector<Mat> offs(3);
			//split(offsetMap, offs);
			//std::vector<Mat> noffs(3);
			//split(newOffsetMapFiner, noffs);
			//noffs[2] = offs[2];
			//merge(noffs, newOffsetMapFiner); 
			Mat &newOffsetMapFiner = offsetMap;
			for(int k = 0; k < numberOfIterationPerScale; k++){
				//Propagation
  				propagation(tempSource, tempTargetFiner, newOffsetMapFiner, k%2);

				//RandomSearch
				randomSearch(tempSource, tempTargetFiner, newOffsetMapFiner);
			}

			//merge Offsetmap
			//updateOffset(offsetMap, newOffsetMapFiner, tempTargetFiner, tempSource);
		}
		Mat newOffset = Mat(offsetMap.rows * 2, offsetMap.cols * 2, offsetMap.type());

		expandOffset(offsetMap, newOffset);
		offsetMap = newOffset.clone();
		tempSource = sourceImage.clone();

		//vote to new scale image
		scale(tempSource, i - 1);
		vote(tempSource, offsetMap, tempTargetFiner);

		sourceImageRGB = displayRegartImage(tempTargetFiner, orgSize);
		cvWaitKey(33);

		//
		if (i == numberOfScales){
			for (int row = 0; row < offsetMap.rows; row++) {
				for (int col = 0; col < offsetMap.cols; col++) {
					if (offsetMap.at<Vec3f>(row, col).val[2] == 255){
						offsetMap.at<Vec3f>(row, col).val[2] = 0;
					}
				}
			}
		}
	}
#pragma endregion
}

void MyPatchMatch::imageImageCopy2(Mat& sourceImageRGB, Mat targetImageRGB){
	Mat sourceImage, targetImage, newOffsetMap;

	sourceImageRGB.convertTo(sourceImage,CV_32FC3,1/255.f);
	cvtColor(sourceImage,sourceImage,CV_RGB2Lab);
	targetImageRGB.convertTo(targetImage,CV_32FC3,1/255.f);
	cvtColor(targetImage,targetImage,CV_RGB2Lab);

	Mat offsetMap(targetImage.rows, targetImage.cols, CV_32FC3);
	initialOffset(offsetMap, true);
	//debugShowOffset("initial", offsetMap);

	Size orgSize = Size(targetImage.size());
	Mat tempTarget = targetImage.clone();

#pragma region //pyrdown
	for(int i = 0; i < numberOfScales; i++){
		pyrDown(tempTarget, tempTarget);
		newOffsetMap = Mat(tempTarget.rows,tempTarget.cols, offsetMap.type());
		for(int row = 0; row < newOffsetMap.rows; row++){
			int b = row * 2;
			if (b >= offsetMap.rows - 1)
				b = offsetMap.rows - 2;
			for (int col = 0; col < newOffsetMap.cols ;col++)
			{
				int a = col * 2;
				if (a >= offsetMap.cols - 1)
					a = offsetMap.cols - 2;


				newOffsetMap.at<Vec3f>(row, col).val[0] = offsetMap.at<Vec3f>(b,a).val[0] / 2;
				newOffsetMap.at<Vec3f>(row, col).val[1] = offsetMap.at<Vec3f>(b,a).val[1] / 2;

				if ((offsetMap.at<Vec3f>(b, a).val[2] == 255) || (offsetMap.at<Vec3f>(b + 1, a).val[2] == 255)
					|| (offsetMap.at<Vec3f>(b, a + 1).val[2] == 255) || (offsetMap.at<Vec3f>(b + 1 , a + 1).val[2] == 255))
				{
					if (offsetMap.at<Vec3f>(b + 1, a).val[2] == 255)
					{
						newOffsetMap.at<Vec3f>(row, col).val[0] = offsetMap.at<Vec3f>(b + 1, a).val[0] / 2;
						newOffsetMap.at<Vec3f>(row, col).val[1] = offsetMap.at<Vec3f>(b + 1, a).val[1] / 2;
					}
					else if (offsetMap.at<Vec3f>(b, a + 1).val[2] == 255)
					{
						newOffsetMap.at<Vec3f>(row, col).val[0] = offsetMap.at<Vec3f>(b, a + 1).val[0] / 2;
						newOffsetMap.at<Vec3f>(row, col).val[1] = offsetMap.at<Vec3f>(b, a + 1).val[1] / 2;
					}
					else if (offsetMap.at<Vec3f>(b + 1, a + 1).val[2] == 255)
					{
						newOffsetMap.at<Vec3f>(row, col).val[0] = offsetMap.at<Vec3f>(b + 1, a + 1).val[0] / 2;
						newOffsetMap.at<Vec3f>(row, col).val[1] = offsetMap.at<Vec3f>(b + 1, a + 1).val[1] / 2;
					}
					newOffsetMap.at<Vec3f>(row, col).val[2] = 255;
				}
				else
					newOffsetMap.at<Vec3f>(row, col).val[2] = 0;
			}
		}
		offsetMap = newOffsetMap.clone();
	}
#pragma endregion


	Mat tempSource = sourceImage.clone();
	scale(tempSource, numberOfScales);

	Mat tempTargetFiner;
	tempTarget.convertTo(tempTargetFiner, CV_32FC3, 1);

#pragma region //pyrup
	for(int i = numberOfScales; i > 0; i--){
		w = i * 50;
		if(i == numberOfScales){
			for (int k = 0; k < numberOfIterationPerScale; k++){
				//propagation
				propagation(tempSource, tempTargetFiner, offsetMap, (k)%2);

				//random  Search
				randomSearch(tempSource, tempTargetFiner, offsetMap);
			}
		}
		else{
			//Mat newOffsetMapFiner(offsetMap.rows, offsetMap.cols, offsetMap.type());
			//initialOffset(newOffsetMapFiner, false);
			//std::vector<Mat> offs(3);
			//split(offsetMap, offs);
			//std::vector<Mat> noffs(3);
			//split(newOffsetMapFiner, noffs);
			//noffs[2] = offs[2];
			//merge(noffs, newOffsetMapFiner); 
			Mat &newOffsetMapFiner = offsetMap;
			for(int k = 0; k < numberOfIterationPerScale; k++){
				//Propagation
  				propagation(tempSource, tempTargetFiner, newOffsetMapFiner, k%2);

				//RandomSearch
				randomSearch(tempSource, tempTargetFiner, newOffsetMapFiner);
			}

			//merge Offsetmap
			//updateOffset(offsetMap, newOffsetMapFiner, tempTargetFiner, tempSource);
		}
		Mat newOffset = Mat(offsetMap.rows * 2, offsetMap.cols * 2, offsetMap.type());

		expandOffset(offsetMap, newOffset);
		offsetMap = newOffset.clone();
		tempSource = sourceImage.clone();

		//vote to new scale image
		scale(tempSource, i - 1);
		vote(tempSource, offsetMap, tempTargetFiner);

		sourceImageRGB = displayRegartImage(tempTargetFiner, orgSize);
		cvWaitKey(33);

		//
		if (i == numberOfScales){
			for (int row = 0; row < offsetMap.rows; row++) {
				for (int col = 0; col < offsetMap.cols; col++) {
					if (offsetMap.at<Vec3f>(row, col).val[2] == 255){
						offsetMap.at<Vec3f>(row, col).val[2] = 0;
					}
				}
			}
		}
	}
#pragma endregion
}
void MyPatchMatch::randomSearch(Mat source, Mat target, Mat& offset){
	for(int row = 0; row < target.rows; row++){
		for(int col = 0; col < target.cols; col++){
			if(offset.at<Vec3f>(row, col)[2] == 255){

			}
			else{
				float rx = rand() % 2000 / 1000.f - 1.f;
				float ry= rand() % 2000 /1000.f - 1.f;
				int times = 0;
				while(true){
					int offsetX = rx * pow(0.25, times) * w;//target.cols;
					int offsetY = ry * pow(0.25, times) * w;//target.rows;
					if(offsetX == 0 && offsetY == 0)
						break;
					int tempX = offset.at<Vec3f>(row, col).val[0] + offsetX;
					int tempY = offset.at<Vec3f>(row, col).val[1] + offsetY;

					if(tempX >= 0 && tempX < target.cols && tempY >= 0 && tempY < target.rows){
						Mat targetPatch = getPatch(row, col, target);
						Mat oldSourcePatch = getPatch(offset.at<Vec3f>(row, col).val[1], offset.at<Vec3f>(row, col).val[0], source);
						Mat newSourcePatch = getPatch(tempY, tempX, source);

						float dis1 = computeDistance(targetPatch, oldSourcePatch);
						float dis2 = computeDistance(targetPatch, newSourcePatch);
						if(dis2 <= dis1){
							offset.at<Vec3f>(row, col).val[0] = tempX;
							offset.at<Vec3f>(row, col).val[1] = tempY;
						}
					}
					times++;
				}
			}

		}
	}

}

void MyPatchMatch::propagation(Mat source, Mat target, Mat& offset, bool odd){

	int aew = target.cols, aeh = target.rows;       /* Effective width and height (possible upper left corners of patches). */
	int bew = source.cols, beh = source.rows;
	int ystart = 0, yend = aeh, ychange = 1;
    int xstart = 0, xend = aew, xchange = 1;
    if (odd) {
      xstart = xend-1; xend = -1; xchange = -1;
      ystart = yend-1; yend = -1; ychange = -1;
    }
	for (int ay = ystart; ay != yend; ay += ychange) {
		for (int ax = xstart; ax != xend; ax += xchange) { 
			if(offset.at<Vec3f>(ay, ax).val[2] == 255 )
				continue;
			int sourceRow = offset.at<Vec3f>(ay, ax).val[1];
			int sourceCol = offset.at<Vec3f>(ay, ax).val[0];
			Mat targetPatch = getPatch(ay, ax, target);
			Mat sourcePatch = getPatch(sourceRow, sourceCol, source);
			float minDis = computeDistance(targetPatch, sourcePatch);
			if ((unsigned) (ay - ychange) < (unsigned) aeh) {
				int x = offset.at<Vec3f>(ay - ychange, ax).val[0];
				int y = offset.at<Vec3f>(ay - ychange, ax).val[1];
				y += ychange;
				if((unsigned) y <= (unsigned) beh){
					Mat sourcePatchUpDown = getPatch(y, x, source);
					float dis2 = computeDistance(targetPatch, sourcePatchUpDown);
					if(dis2 < minDis){
						offset.at<Vec3f>(ay, ax).val[0] = x;
						offset.at<Vec3f>(ay, ax).val[1] = y;
						minDis = dis2;
					}
				}
			}
			if ((unsigned) (ax - xchange) < (unsigned) aew){
				int x = offset.at<Vec3f>(ay, ax - xchange).val[0];
				int y = offset.at<Vec3f>(ay, ax - xchange).val[1];
				x += xchange;
				if ((unsigned) x < (unsigned) bew){
					Mat sourcePatchLeftRight = getPatch(y, x, source);
					float dis1 = computeDistance(targetPatch, sourcePatchLeftRight);
					if(dis1 <= minDis){
						offset.at<Vec3f>(ay, ax).val[0] = x;
						offset.at<Vec3f>(ay, ax).val[1] = y;
						minDis = dis1;
					}
				}
			}		
		}
	}
}

float MyPatchMatch::computeDistance(Mat A, Mat B){
	float dis = 0;
	for(int row = 0; row < A.rows; row++){
		for(int col = 0; col < A.cols; col++){
			float l, a, b;
			float l1, l2, a1, a2, b1, b2;
			l1 = A.at<Vec3f>(row, col).val[0];
			a1 = A.at<Vec3f>(row, col).val[1];
			b1 = A.at<Vec3f>(row, col).val[2];
			l2 = B.at<Vec3f>(row, col).val[0];
			a2 = B.at<Vec3f>(row, col).val[1];
			b2 = B.at<Vec3f>(row, col).val[2];			
			l = A.at<Vec3f>(row, col).val[0] -  B.at<Vec3f>(row, col).val[0];
			a = A.at<Vec3f>(row, col).val[1] -  B.at<Vec3f>(row, col).val[1];
			b = A.at<Vec3f>(row, col).val[2] -  B.at<Vec3f>(row, col).val[2];
			dis += 	l * l + a * a + b * b;
			//dis += sqrt(l * l + a * a + b * b);
		}
	}
	return dis;
}

//also used in old inpaint method
Mat MyPatchMatch::getPatch(int centerRow, int centerCol, Mat source){
	int topLeftRow = centerRow - PATCHRADIUS;
	int topLeftCol = centerCol - PATCHRADIUS;

	if(topLeftRow < 0)
		topLeftRow = 0;
	if(topLeftRow > source.rows - PATCHSIZE)
		topLeftRow = source.rows - PATCHSIZE;

	if(topLeftCol < 0)
		topLeftCol = 0;
	if(topLeftCol > source.cols - PATCHSIZE)
		topLeftCol = source.cols - PATCHSIZE;

	return source(Range(topLeftRow, topLeftRow + PATCHSIZE), Range(topLeftCol, topLeftCol + PATCHSIZE));
}

void MyPatchMatch::scale(Mat& image, int scaleTime){
	for(int i = 0; i < scaleTime; i++){
		pyrDown(image, image);
	}
}

void MyPatchMatch::initialOffset(Mat &offset, bool ifReserve){
	int Offset = 0;
	int staticUpLeftCornerX = newTopLeftX + Offset;
	int staticUpLeftCornerY = newTopLeftY;
	newTopLeftY += 10;
	Rect rect = getImageCopyMaskRect();
	
	for(int row = 0; row < offset.rows; row++){
		for(int col = 0; col < offset.cols; col++){
			if(ifReserve && (col >= newTopLeftX + Offset && col <  newTopLeftX - Offset + rect.width)
				&& (row >= newTopLeftY + Offset && row <  newTopLeftY - Offset + rect.height)){
					offset.at<Vec3f>(row, col).val[0] = col + rect.x - newTopLeftX;
					offset.at<Vec3f>(row, col).val[1] = row + rect.y - newTopLeftY;
					offset.at<Vec3f>(row, col).val[2] = 255;
			}
			//if(ImageCopyMask[row * offset.cols + col > 0]){
			//	offset.at<Vec3f>(row, col).val[0] = col + rect.x - newTopLeftX;
			//	offset.at<Vec3f>(row, col).val[1] = row + rect.y - newTopLeftY;
			//	offset.at<Vec3f>(row, col).val[2] = 255;
			//}
			else{
				int randomX = rand() % (offset.cols - 1);
				int randomY = rand() % (offset.rows - 1);
				offset.at<Vec3f>(row, col).val[0] = randomX;
				offset.at<Vec3f>(row, col).val[1] = randomY;
				offset.at<Vec3f>(row, col).val[2] = 0;
			}
		}
	}
}

void MyPatchMatch::showImageCopyMaskImage(Mat &image){
	Mat temp;
	image.copyTo(temp);
	for(int row = 0; row < image.rows; row++){
		for(int col = 0; col < image.cols; col++){
			if(ImageCopyMask[row * image.cols + col] > 0){
				image.at<cv::Vec3b>(row, col).val[0] = (image.at<cv::Vec3b>(row, col).val[0] + 255) / 2;
			}
		}	
	}
	//addCircleToImage(image, ax, ay);
	//imshow("image", image);
}

void MyPatchMatch::addToImageCopyMask(int ax, int ay, int rows, int cols){
	int radius = ImageCopyRadius;
	int centerX = ax;
	int centerY = ay;
	imageCols = cols;
	imageRows = rows;
	for(int row = -radius; row < radius + 1; row++){
		for(int col = -radius; col < radius + 1; col++){
			if(centerX + col < 0 || centerY + row < 0 || centerX + col >= cols || centerY + row >= rows)
				continue;
			if(row * row + col * col < radius * radius){	
				ImageCopyMask[(centerY + row) * cols + centerX + col] = 1;
			}
		}
	}
}

void MyPatchMatch::zeroImageCopyMask(int length, bool newImage){
	if(newImage){
		if(ImageCopyMask != NULL)
			delete ImageCopyMask;
		ImageCopyMask = new char[length];
	}

	for(int i = 0; i < length; i++){
		ImageCopyMask[i] = 0;
	}
}

Rect MyPatchMatch::getImageCopyMaskRect(){
	int xmin = imageCols, xmax = 0;
	int ymin = imageRows, ymax = 0;
	if(xmin == 0 || ymin == 0)
		return Rect(0, 0, 0, 0);
	for(int row = 0; row < imageRows; row++){
		for(int col = 0; col < imageCols; col++){
			if(ImageCopyMask[row * imageCols + col] > 0){
				if(row > ymax)
					ymax = row;
				if(row < ymin)
					ymin = row;
				if(col > xmax)
					xmax = col;
				if(col < xmin)
					xmin = col;
			}
		}
	}
	return Rect(xmin, ymin, xmax - xmin, ymax - ymin);
}

void MyPatchMatch::expandOffset(Mat oldOffset, Mat& newOffset){
	for(int row = 0; row < oldOffset.rows; row++){
		for(int col = 0; col < oldOffset.cols; col++){
			float disRow = (oldOffset.at<Vec3f>(row, col).val[1] - row) * 2;
			float disCol = (oldOffset.at<Vec3f>(row, col).val[0] - col) * 2;
			newOffset.at<Vec3f>(row * 2, col * 2)[0] = col * 2 + disCol;
			newOffset.at<Vec3f>(row * 2, col * 2)[1] = row * 2 + disRow;;
			newOffset.at<Vec3f>(row * 2, col * 2)[2] = oldOffset.at<Vec3f>(row, col)[2];
			newOffset.at<Vec3f>(row * 2 + 1, col * 2)[0] = col * 2 + disCol;
			newOffset.at<Vec3f>(row * 2 + 1, col * 2)[1] = row * 2 + 1 + disRow;;
			newOffset.at<Vec3f>(row * 2 + 1, col * 2)[2] = oldOffset.at<Vec3f>(row, col)[2];
			newOffset.at<Vec3f>(row * 2, col * 2 + 1)[0] = col * 2 + 1 + disCol;
			newOffset.at<Vec3f>(row * 2, col * 2 + 1)[1] = row * 2 + disRow;;
			newOffset.at<Vec3f>(row * 2, col * 2 + 1)[2] = oldOffset.at<Vec3f>(row, col)[2];
			newOffset.at<Vec3f>(row * 2 + 1, col * 2 + 1)[0] = col * 2 + 1 + disCol;
			newOffset.at<Vec3f>(row * 2 + 1, col * 2 + 1)[1] = row * 2 + 1 + disRow;;
			newOffset.at<Vec3f>(row * 2 + 1, col * 2 + 1)[2] = oldOffset.at<Vec3f>(row, col)[2];
		}
	}
}



//also used in old inpaint method
void MyPatchMatch::updateOffset(Mat& offsetMap, Mat newOffsetMap, Mat target, Mat source){
	for(int row = 0; row < offsetMap.rows; row++){
		for(int col = 0; col < offsetMap.cols; col++){
			if(offsetMap.at<Vec3f>(row, col)[2] == 255){
			}
			else{
				Mat targetPatch = getPatch(row, col, target);
				Mat oldSourcePatch = getPatch(offsetMap.at<Vec3f>(row, col).val[1], offsetMap.at<Vec3f>(row, col).val[0], source);
				Mat newSourcePatch = getPatch(newOffsetMap.at<Vec3f>(row, col).val[1], newOffsetMap.at<Vec3f>(row, col).val[0], source);

				float dis1 = computeDistance(targetPatch, oldSourcePatch);
				float dis2 = computeDistance(targetPatch, newSourcePatch);

				if(dis2 < dis1){
				 	offsetMap.at<Vec3f>(row, col).val[0] = newOffsetMap.at<Vec3f>(row, col).val[0];
					offsetMap.at<Vec3f>(row, col).val[1] = newOffsetMap.at<Vec3f>(row, col).val[1];
				}
			}

		}
	}

}

Mat MyPatchMatch::displayRegartImage(Mat &Image, Size orgSize){
	Mat tempImage = Image.clone();
	cvtColor(tempImage, tempImage, CV_Lab2RGB);
	tempImage.convertTo(tempImage,CV_8UC3,255,0);
	Mat resizeImage;
    resize(tempImage, resizeImage, orgSize, 0, 0, INTER_NEAREST );
	
	imshow("image", resizeImage);
	return tempImage;
}

void MyPatchMatch::getAndShowImageCopyMovedImage(int ax, int ay, Mat& image, bool ifImageCopy){
	Mat temp;
	image.copyTo(temp);
	Rect targetRect = getImageCopyMaskRect();
	newTopLeftX = oldTopLeftX + ax - mouseDownX;
	newTopLeftY = oldTopLeftY + ay - mouseDownY;

	if(newTopLeftX < 0)
		newTopLeftX = 0;
	else if(newTopLeftX > image.cols - targetRect.width)
		newTopLeftX = image.cols - targetRect.width;

	if(newTopLeftY < 0)
		newTopLeftY = 0;
	else if(newTopLeftY > image.rows - targetRect.height)
		newTopLeftY = image.rows - targetRect.height;


	//Rect newRect(newTopLeftX, newTopLeftY, targetRect.width, targetRect.height);
	//image(targetRect).copyTo(temp(newRect));

	for(int row = 0; row < targetRect.height; row++){
		for(int col = 0; col < targetRect.width; col++){
			if(ImageCopyMask[(row + targetRect.y) * image.cols + col + targetRect.x] > 0)
				temp.at<Vec3b>(row + newTopLeftY, col + newTopLeftX) = image.at<Vec3b>(row + targetRect.y, col + targetRect.x);
		}
	}

	if(ifImageCopy){
		switch(mode){
		//imageImageCopy(image, temp);
		case CPU:
			imageInpaintCopy(image, temp);
			break;
		case GPU_JUMPFLOODING:
			imageInpaintCopyCL_JF(image, temp);
			break;
		}
	}
	else{
		temp.copyTo(image);
		//imshow("image", temp);
		//cvWaitKey(20);
	}

}

void MyPatchMatch::addCircleToImage(cv::Mat &image, int ax, int ay){
	int radius;
	int b, g, r;

	//ImageCopy
	b = 255;
	g = 0;
	r = 0;
	radius = ImageCopyRadius;

	int centerX = ax;
	int centerY = ay;
	for(int row = -radius; row < radius + 1; row++){
		for(int col = -radius; col < radius + 1; col++){
			if(centerX + col < 0 || centerY + row < 0 || centerX + col >= image.cols || centerY + row >= image.rows)
				continue;
			if(row * row + col * col < radius * radius){	
				image.at<cv::Vec3b>(centerY + row, centerX + col).val[0] = b;
				image.at<cv::Vec3b>(centerY + row, centerX + col).val[1] = g;
				image.at<cv::Vec3b>(centerY + row, centerX + col).val[2] = r;
			}
		}
	}
}

void MyPatchMatch::vote(Mat source, Mat offset, Mat &result){
	Mat record(source.rows, source.cols, CV_32FC1, Scalar::all(0));
	Mat target(source.rows, source.cols, CV_32FC3, Scalar::all(0));

	for(int row = 0; row < target.rows; row++){
		for(int col = 0; col < target.cols; col++){	
			Mat recordPatch = getPatch(row, col, record);
			recordPatch +=1;

			int sourceRow = offset.at<Vec3f>(row, col).val[1];
			int sourceCol = offset.at<Vec3f>(row, col).val[0];

			Mat targetPatch = getPatch(row, col, target);
			Mat sourcePatch = getPatch(sourceRow, sourceCol, source);
			for(int y = 0; y < PATCHSIZE; y++){
				for(int x = 0; x < PATCHSIZE; x++){
					targetPatch.at<Vec3f>(y, x).val[0] += sourcePatch.at<Vec3f>(y, x).val[0];
					targetPatch.at<Vec3f>(y, x).val[1] += sourcePatch.at<Vec3f>(y, x).val[1];
					targetPatch.at<Vec3f>(y, x).val[2] += sourcePatch.at<Vec3f>(y, x).val[2];
				}
			}
		}
	}
	//for(int row = 0; row < target.rows; row++){
	//	for(int col = 0; col < target.cols; col++){	
	//		int a = record.at<Vec3f>(row, col).val[0];
	//		target.at<Vec3f>(row, col).val[0] /= record.at<Vec3f>(row, col).val[0];
	//		target.at<Vec3f>(row, col).val[1] /= record.at<Vec3f>(row, col).val[0];
	//		target.at<Vec3f>(row, col).val[2] /= record.at<Vec3f>(row, col).val[0];
	//	}
	//}
	//target.copyTo(result);

	std::vector<Mat> lab(3);
	split(target,lab);
	lab[0] /= record;
	lab[1] /= record;
	lab[2] /= record;
	merge(lab,target); 
	
	//imshow("record", record);
	target.copyTo(result);
}




//////////////////////////////////////////////old inpaint method/////////////////////////////////////////////////////////////
void MyPatchMatch::imageInpaint2(Mat& sourceImageRGB, Mat targetImageRGB){
	Mat sourceImage, targetImage, newOffsetMap;

	sourceImageRGB.convertTo(sourceImage,CV_32FC3,1/255.f);
	cvtColor(sourceImage,sourceImage,CV_RGB2Lab);
	targetImageRGB.convertTo(targetImage,CV_32FC3,1/255.f);
	cvtColor(targetImage,targetImage,CV_RGB2Lab);


	Mat inpaintMask[10];
	Mat Target[10];
	Mat Source[10];

	//targetImage.copyTo(inpaintMask[0]);
	//erodeMask(targetImage.rows, targetImage.cols);

	inpaintMask[0] = Mat(sourceImageRGB.size(), CV_32FC3);

	for(int row = 0; row < targetImage.rows; row++){
		for(int col = 0; col < targetImage.cols; col++){
			if(ImageInpaintMask[row * targetImage.cols + col] > 0)
				inpaintMask[0].at<cv::Vec3f>(row, col).val[0] = HOLE;
			else 
				inpaintMask[0].at<cv::Vec3f>(row, col).val[0] = SOURCE;
			inpaintMask[0].at<cv::Vec3f>(row, col).val[1] = 0;
			inpaintMask[0].at<cv::Vec3f>(row, col).val[2] = 0;
		}
	}
	imshow("origin", inpaintMask[0]);

	Size orgSize = Size(targetImage.size());
	Target[0] = targetImage.clone();
	Source[0] = targetImage.clone();
	
	cvWaitKey(100000);

	for(int i = 0; i < numberOfScales; i++){
		pyrDown(inpaintMask[i], inpaintMask[i + 1]);
		pyrDown(Target[i], Target[i + 1]);
		pyrDown(Source[i], Source[i + 1]);
		inpaintMask[i + 1].setTo(0);
		pyrDownMask(inpaintMask[i], inpaintMask[i + 1]);
		debugImage(inpaintMask[i + 1], "pyrdown");
		cvWaitKey();
	}



	Mat offsetMap(inpaintMask[numberOfScales].size(), CV_32FC3, Scalar::all(0));
	initialImageInpaintOffset(offsetMap, inpaintMask[numberOfScales]);
	initHole(Target[numberOfScales], offsetMap);

	debugShowOffset("init", offsetMap);
	debugImage(Source[numberOfScales], "initial");
	cvWaitKey(100000);

	w = max(sourceImageRGB.rows, sourceImageRGB.cols);
	for(int i = numberOfScales; i > 0; i--){
		//w = i * 50;
		if(i == numberOfScales){
			for (int k = 1; k < numberOfIterationPerScale + i * 5; k++){
				propagation(Source[i], Target[i], offsetMap, (k)%2);
				randomSearch(Source[i], Target[i], offsetMap);

				vote2(Source[i], offsetMap, Target[i], inpaintMask[i]);
				debugImage(Target[i], "qwe");
				cvWaitKey(100000);
			}
		}
		else{
			Mat newOffsetMapFiner(offsetMap.rows, offsetMap.cols, offsetMap.type());
			//make sure that the constraints get written in newoffsetmap
			initialImageInpaintOffset(newOffsetMapFiner, inpaintMask[i]);
			//initialOffset(newOffsetMapFiner, false);

			for(int k = 1; k < numberOfIterationPerScale + i * 5; k++){
				//Propagation
  				propagation(Source[i], Target[i], newOffsetMapFiner, k%2);

				//RandomSearch
				randomSearch(Source[i], Target[i], newOffsetMapFiner);

				vote2(Source[i], offsetMap, Target[i], inpaintMask[i]);
				debugImage(Target[i], "qwe");
				cvWaitKey(100000);
			}
			//merge Offsetmap
			updateOffset(offsetMap, newOffsetMapFiner, Target[i], Source[i]);
		}

		Mat newOffset;
		pyrUpOffset(offsetMap, inpaintMask[i - 1], newOffset);
		offsetMap = newOffset.clone();
		debugShowOffset("votebefore", offsetMap);
		cvWaitKey(500);
		vote2(Source[i - 1], offsetMap, Target[i - 1], inpaintMask[i - 1]);

		sourceImageRGB = displayRegartImage(Target[i - 1], orgSize);
	}
}

void MyPatchMatch::imageInpaint3(Mat& sourceImageRGB, Mat targetImageRGB){
	Mat sourceImage, targetImage, newOffsetMap;
	Mat copySourceImageRGB = sourceImageRGB.clone();
	sourceImageRGB.convertTo(sourceImage,CV_32FC3,1/255.f);
	cvtColor(sourceImage,sourceImage,CV_RGB2Lab);
	targetImageRGB.convertTo(targetImage,CV_32FC3,1/255.f);
	cvtColor(targetImage,targetImage,CV_RGB2Lab);

	Mat inpaintMask[10];
	Mat Target[10];
	Mat Source[10];
	inpaintMask[0] = Mat(sourceImageRGB.size(), CV_32FC3);
	
	for(int row = 0; row < targetImage.rows; row++){
		for(int col = 0; col < targetImage.cols; col++){
			if(ImageInpaintMask[row * targetImage.cols + col] > 0)
				inpaintMask[0].at<cv::Vec3f>(row, col).val[0] = HOLE;
			else if(row < PATCHRADIUS || row > targetImage.rows - 1 - PATCHRADIUS
				||  col < PATCHRADIUS || col > targetImage.cols - 1 - PATCHRADIUS){
				inpaintMask[0].at<cv::Vec3f>(row, col).val[0] = OTHER;
			}
			else 
				inpaintMask[0].at<cv::Vec3f>(row, col).val[0] = SOURCE;
			inpaintMask[0].at<cv::Vec3f>(row, col).val[1] = 0;
			inpaintMask[0].at<cv::Vec3f>(row, col).val[2] = 0;
		}
	}
	Size orgSize = Size(targetImage.size());
	Target[0] = targetImage.clone();
	Source[0] = targetImage.clone();

	for(int i = 0; i < numberOfScales; i++){
		
		pyrDown(inpaintMask[i], inpaintMask[i + 1]);
		pyrDown(Target[i], Target[i + 1]);
		pyrDown(Source[i], Source[i + 1]);

		pyrDownMask(inpaintMask[i], inpaintMask[i + 1]);
		
		imshow("origin", inpaintMask[i+1]);
		cvWaitKey();
		if(Source[i].rows < 15 || Source[i].cols < 15){
			numberOfScales = i;
			break;
		}
	}

	Mat offsetMap(inpaintMask[numberOfScales].size(), CV_32FC3, Scalar::all(0));
	
	initialImageInpaintOffset(offsetMap, inpaintMask[numberOfScales]);
	w = max(sourceImageRGB.rows, sourceImageRGB.cols);
	for(int i = numberOfScales; i >= 0; i--){
		setOffsetMask(offsetMap, inpaintMask[i]);
		initHole(Target[i], offsetMap);
		debugImage(Source[i], "s");
		waitKey();
		for (int k = 1; k < numberOfIterationPerScale + i * 5; k++){
			generateHole(Target[i], Source[i], offsetMap, k % 2);
			Mat show;
			show.setTo(0);
			resize(Target[i], show, orgSize);
			debugImage(show, "show");
			cvWaitKey(50);
		}
		if(i > 0){
			Mat newOffset;
			pyrUpOffset(offsetMap, inpaintMask[i - 1], newOffset);
			offsetMap = newOffset.clone();
		}
	}
}

void MyPatchMatch::randomSearchHole(Mat source, Mat target, Mat& offset, int row, int col){
	float rx = rand() % 2000 / 1000.f - 1.f;
	float ry= rand() % 2000 /1000.f - 1.f;
	int times = 0;
	while(true){
		int offsetX = rx * pow(0.5, times) * target.cols;
		int offsetY = ry * pow(0.5, times) * target.rows;
		if(offsetX == 0 && offsetY == 0)
			break;
		int tempX = offset.at<Vec3f>(row, col).val[0] + offsetX;
		int tempY = offset.at<Vec3f>(row, col).val[1] + offsetY;

		if(tempX < 0)
			tempX = 0;
		if(tempX > target.cols - 1)
			tempX = target.cols - 1;
		if(tempY < 0)
			tempY = 0;
		if(tempY > target.rows - 1)
			tempY = target.rows - 1;

		if(tempX >= 0 && tempX < target.cols && tempY >= 0 && tempY < target.rows){
			Mat targetPatch = getPatch(row, col, target);
			Mat oldSourcePatch = getPatch(offset.at<Vec3f>(row, col).val[1], offset.at<Vec3f>(row, col).val[0], source);
			Mat newSourcePatch = getPatch(tempY, tempX, source);

			float dis1 = computeDistanceWithHole(targetPatch, oldSourcePatch, offset, 
				offset.at<Vec3f>(row, col).val[1] - PATCHRADIUS, offset.at<Vec3f>(row, col).val[0] - PATCHRADIUS);
			float dis2 = computeDistanceWithHole(targetPatch, newSourcePatch, offset, tempY - PATCHRADIUS, tempX - PATCHRADIUS);
			if(dis2 <= dis1){
				offset.at<Vec3f>(row, col).val[0] = tempX;
				offset.at<Vec3f>(row, col).val[1] = tempY;
			}
		}
		times++;
	}

}

void MyPatchMatch::propagationHole(Mat source, Mat target, Mat& offset, int ay, int ax, bool odd){

	int aew = target.cols, aeh = target.rows;       /* Effective width and height (possible upper left corners of patches). */
	int bew = source.cols, beh = source.rows;
	int ystart = 0, yend = aeh, ychange = 1;
    int xstart = 0, xend = aew, xchange = 1;
    if (!odd) {
      xstart = xend-1; xend = -1; xchange = -1;
      ystart = yend-1; yend = -1; ychange = -1;
    }

	int sourceRow = offset.at<Vec3f>(ay, ax).val[1];
	int sourceCol = offset.at<Vec3f>(ay, ax).val[0];
	Mat targetPatch = getPatch(ay, ax, target);
	Mat sourcePatch = getPatch(sourceRow, sourceCol, source);
	float minDis = computeDistanceWithHole(targetPatch, sourcePatch, offset, sourceRow - PATCHRADIUS, sourceCol - PATCHRADIUS);
	//float minDis = computeDistance(targetPatch, sourcePatch);
	if ((unsigned) (ay - ychange) < (unsigned) aeh) {
		int x = offset.at<Vec3f>(ay - ychange, ax).val[0];
		int y = offset.at<Vec3f>(ay - ychange, ax).val[1];
		y += ychange;
		if((unsigned) y <= (unsigned) beh){
			Mat sourcePatchUpDown = getPatch(y, x, source);
			float dis2 = computeDistanceWithHole(targetPatch, sourcePatchUpDown, offset, y - PATCHRADIUS, x - PATCHRADIUS);
			//float dis2 = computeDistance(targetPatch, sourcePatchUpDown);
			if(dis2 < minDis){
				offset.at<Vec3f>(ay, ax).val[0] = x;
				offset.at<Vec3f>(ay, ax).val[1] = y;
				minDis = dis2;
			}
		}
	}
	if ((unsigned) (ax - xchange) < (unsigned) aew){
		int x = offset.at<Vec3f>(ay, ax - xchange).val[0];
		int y = offset.at<Vec3f>(ay, ax - xchange).val[1];
		x += xchange;
		if ((unsigned) x < (unsigned) bew){
			Mat sourcePatchLeftRight = getPatch(y, x, source);
			float dis1 = computeDistanceWithHole(targetPatch, sourcePatchLeftRight, offset, y - PATCHRADIUS, x - PATCHRADIUS);
			//float dis1 = computeDistance(targetPatch, sourcePatchLeftRight);
			if(dis1 <= minDis){
				offset.at<Vec3f>(ay, ax).val[0] = x;
				offset.at<Vec3f>(ay, ax).val[1] = y;
				minDis = dis1;
			}
		}
	}		
}

float MyPatchMatch::computeDistanceWithHole(Mat A, Mat B, Mat offset, int low_row, int left_col){
	float dis = 0;

	if(low_row < 0)
		low_row = 0;
	if(low_row > offset.rows - PATCHSIZE)
		low_row = offset.rows - PATCHSIZE;
	if(left_col < 0)
		left_col = 0;
	if(left_col > offset.cols - PATCHSIZE)
		left_col = offset.cols - PATCHSIZE;

	for(int row = 0; row < A.rows; row++){
		for(int col = 0; col < A.cols; col++){
			float l, a, b;
			float l1, l2, a1, a2, b1, b2;
			l1 = A.at<Vec3f>(row, col).val[0];
			a1 = A.at<Vec3f>(row, col).val[1];
			b1 = A.at<Vec3f>(row, col).val[2];
			l2 = B.at<Vec3f>(row, col).val[0];
			a2 = B.at<Vec3f>(row, col).val[1];
			b2 = B.at<Vec3f>(row, col).val[2];			
			l = A.at<Vec3f>(row, col).val[0] -  B.at<Vec3f>(row, col).val[0];
			a = A.at<Vec3f>(row, col).val[1] -  B.at<Vec3f>(row, col).val[1];
			b = A.at<Vec3f>(row, col).val[2] -  B.at<Vec3f>(row, col).val[2];
			dis += 	l * l + a * a + b * b;
			if(offset.at<Vec3f>(low_row + row, left_col + col)[2] == 0)
				dis +=1000000;
			//dis += sqrt(l * l + a * a + b * b);
		}
	}
	return dis;
}

void MyPatchMatch::initHole(cv::Mat image, cv::Mat offset){
	for(int row = 0; row < image.rows; row++){
		for(int col = 0; col < image.cols; col++){
			if(offset.at<cv::Vec3f>(row, col)[2] == 0)
				image.at<cv::Vec3f>(row, col) = image.at<cv::Vec3f>(offset.at<cv::Vec3f>(row, col)[1], offset.at<cv::Vec3f>(row, col)[0]);
		}
	}
}

void MyPatchMatch::pyrDownMask(cv::Mat in, cv::Mat &out){
	int rows = (in.rows+1)/2;
	int cols = (in.cols+1)/2;

	out = Mat(rows, cols, in.type());

	for (int i = 0; i< rows; i++){
		for (int j = 0; j< cols; j++){
			int m = i;
			int n = j;

			int m2 = 2*i;
			int m21 = 2*i+1;

			int n2 = 2*j;
			int n21 = 2*j+1;

			if(true){
			if (m21 < in.rows
				&&n21 < in.cols)
			{
				if (in.at<cv::Vec3f>(m2, n2)[0] == HOLE
					||in.at<cv::Vec3f>(m2, n21)[0] == HOLE
					||in.at<cv::Vec3f>(m21, n2)[0] == HOLE
					||in.at<cv::Vec3f>(m21, n21)[0] == HOLE)
					out.at<cv::Vec3f>(i, j)[0] = HOLE;
				else if(in.at<cv::Vec3f>(m2, n2)[0] == OTHER
					||in.at<cv::Vec3f>(m2, n21)[0] == OTHER
					||in.at<cv::Vec3f>(m21, n2)[0] == OTHER
					||in.at<cv::Vec3f>(m21, n21)[0] == OTHER)
					out.at<cv::Vec3f>(i, j)[0] = OTHER;
				else
					out.at<cv::Vec3f>(m, n)[0] = SOURCE;
			}
			else if (m21 < in.rows
				&&n21 == in.cols)
			{
				if (in.at<cv::Vec3f>(m2, n2)[0] == HOLE
					||in.at<cv::Vec3f>(m21, n2)[0] == HOLE)
					out.at<cv::Vec3f>(i, j)[0] = HOLE;
				else if (in.at<Vec3f>(m2, n2)[0] == OTHER
					||in.at<Vec3f>(m21, n2)[0] == OTHER)
					out.at<Vec3f>(m, n) = OTHER;
				else
					out.at<cv::Vec3f>(m, n)[0] = SOURCE;
			}
			else if (m21 == in.rows
				&&n21 < in.cols)
			{
				if (in.at<cv::Vec3f>(m2, n2)[0] == HOLE
					||in.at<cv::Vec3f>(m2, n21)[0] == HOLE)
					out.at<cv::Vec3f>(i, j)[0] = HOLE;
				else if (in.at<cv::Vec3f>(m2, n2)[0] == OTHER
					||in.at<cv::Vec3f>(m2, n21)[0] == OTHER)
					out.at<cv::Vec3f>(i, j)[0] = OTHER;
				else
					out.at<cv::Vec3f>(m, n)[0] = SOURCE;
			}
			else
			{
				if (in.at<cv::Vec3f>(m2, n2)[0] == HOLE)
					out.at<cv::Vec3f>(i, j)[0] = HOLE;
				else if (in.at<cv::Vec3f>(m2, n2)[0] == OTHER)
					out.at<cv::Vec3f>(i, j)[0] = OTHER;
				else
					out.at<cv::Vec3f>(m, n)[0] = SOURCE;
			}


			}

			else{
			if (m21 < in.rows && n21 < in.cols)
			{
				if (in.at<cv::Vec3f>(m2, n2)[0] == SOURCE
					||in.at<cv::Vec3f>(m2, n21)[0] == SOURCE
					||in.at<cv::Vec3f>(m21, n2)[0] == SOURCE
					||in.at<cv::Vec3f>(m21, n21)[0] == SOURCE)
					out.at<cv::Vec3f>(i, j)[0] = SOURCE;
				else if(in.at<cv::Vec3f>(m2, n2)[0] == OTHER
					||in.at<cv::Vec3f>(m2, n21)[0] == OTHER
					||in.at<cv::Vec3f>(m21, n2)[0] == OTHER
					||in.at<cv::Vec3f>(m21, n21)[0] == OTHER)
					out.at<cv::Vec3f>(i, j)[0] = OTHER;
				else
					out.at<cv::Vec3f>(m, n)[0] = HOLE;
			}
			else if (m21 < in.rows
				&&n21 == in.cols)
			{
				if (in.at<cv::Vec3f>(m2, n2)[0] == SOURCE
					||in.at<cv::Vec3f>(m21, n2)[0] == SOURCE)
					out.at<cv::Vec3f>(i, j)[0] = SOURCE;
				else if (in.at<Vec3f>(m2, n2)[0] == OTHER
					||in.at<Vec3f>(m21, n2)[0] == OTHER)
					out.at<Vec3f>(m, n) = OTHER;
				else
					out.at<cv::Vec3f>(m, n)[0] = HOLE;
			}
			else if (m21 == in.rows
				&&n21 < in.cols)
			{
				if (in.at<cv::Vec3f>(m2, n2)[0] == SOURCE
					||in.at<cv::Vec3f>(m2, n21)[0] == SOURCE)
					out.at<cv::Vec3f>(i, j)[0] = SOURCE;
				else if (in.at<cv::Vec3f>(m2, n2)[0] == OTHER
					||in.at<cv::Vec3f>(m2, n21)[0] == OTHER)
					out.at<cv::Vec3f>(i, j)[0] = OTHER;
				else
					out.at<cv::Vec3f>(m, n)[0] = HOLE;
			}
			else
			{
				if (in.at<cv::Vec3f>(m2, n2)[0] == SOURCE)
					out.at<cv::Vec3f>(i, j)[0] = SOURCE;
				else if (in.at<cv::Vec3f>(m2, n2)[0] == OTHER)
					out.at<cv::Vec3f>(i, j)[0] = OTHER;
				else
					out.at<cv::Vec3f>(m, n)[0] = HOLE;
			}
			}
			out.at<cv::Vec3f>(m, n)[1] = 0;
			out.at<cv::Vec3f>(m, n)[2] = 0;
		}
	}
}

void MyPatchMatch::pyrUpOffset(Mat in, Mat mask, Mat &out){
	int rows = mask.rows;
	int cols = mask.cols;

	int c = 0;
	for (int i = 0; i< in.rows; i++){
		for (int j = 0; j< in.cols; j++)
		{
			if(in.at<Vec3f>(i,j)[1] > c)
				c = in.at<Vec3f>(i,j)[1];
		}
	}
	out = Mat(rows, cols, in.type());

	for (int i = 0; i< in.rows; i++){
		for (int j = 0; j< in.cols; j++)
		{
			int m = i;
			int n = j;

			int m2 = 2*i;
			int m21 = 2*i+1;

			int n2 = 2*j;
			int n21 = 2*j+1;

			int disx = (in.at<Vec3f>(m,n)[0] - n) * 2;
			int disy = (in.at<Vec3f>(m,n)[1] - m) * 2;
			if(disy > 0)
				c++;
			if (m21 < out.rows
				&&n21 < out.cols)
			{

				out.at<Vec3f>(m2,n2)[0] = n2 + disx;
				out.at<Vec3f>(m2,n2)[1] = m2 + disy;
				out.at<Vec3f>(m21,n2)[0] = n2 + disx;
				out.at<Vec3f>(m21,n2)[1] = m21 + disy;
				out.at<Vec3f>(m2,n21)[0] = n21 + disx;
				out.at<Vec3f>(m2,n21)[1] = m2 + disy;
				out.at<Vec3f>(m21,n21)[0] = n21 + disx;
				out.at<Vec3f>(m21,n21)[1] = m21 + disy;
			}
			else if (m21 < out.rows
				&&n21 == out.cols)
			{
				out.at<Vec3f>(m2,n2)[0] = n2 + disx;
				out.at<Vec3f>(m2,n2)[1] = m2 + disy;
				out.at<Vec3f>(m21,n2)[0] = n2 + disx;
				out.at<Vec3f>(m21,n2)[1] = m21 + disy;
			}
			else if (m21 == out.rows
				&&n21 < out.cols)
			{
				out.at<Vec3f>(m2,n2)[0] = n2 + disx;
				out.at<Vec3f>(m2,n2)[1] = m2 + disy;
				out.at<Vec3f>(m2,n21)[0] = n21 + disx;
				out.at<Vec3f>(m2,n21)[1] = m2 + disy;
			}
			else
			{
				out.at<Vec3f>(m2,n2)[0] = n2 + disx;
				out.at<Vec3f>(m2,n2)[1] = m2 + disy;
			}
		}
	}

	for (int i = 0; i< out.rows; i++){
		for (int j = 0; j< out.cols; j++)
		{
			if(out.at<Vec3f>(i,j)[0] >= out.cols)
				out.at<Vec3f>(i,j)[0] = out.cols - 1;
			else if(out.at<Vec3f>(i,j)[0] < 0)
				out.at<Vec3f>(i,j)[0] = 0;

			if(out.at<Vec3f>(i,j)[1] >= out.rows)
				out.at<Vec3f>(i,j)[1] = out.rows - 1;
			else if(out.at<Vec3f>(i,j)[1] < 0)
				out.at<Vec3f>(i,j)[1] = 0;
				
		}
	}
	int c2 = 0;
	for (int i = 0; i< out.rows; i++){
		for (int j = 0; j< out.cols; j++)
		{
			if(out.at<Vec3f>(i,j)[1] > c2)
				c2 = out.at<Vec3f>(i,j)[1];
		}
	}

	int b1 = c2;
}

void MyPatchMatch::setOffsetMask(Mat &offset, Mat mask){
	for(int row = 0; row < mask.rows; row++){
		for(int col = 0; col < mask.cols; col++){
			if(mask.at<cv::Vec3f>(row, col).val[0] == HOLE){
				offset.at<Vec3f>(row, col).val[2] = 0;
			}
			else{
				offset.at<Vec3f>(row, col).val[2] = 255;
			}
		}
	}
}

Vec3f MyPatchMatch::generateHolePixel(Mat& target, Mat source, Mat offset, int pixel_row, int pixel_col, bool odd){
	Vec3f *center = new Vec3f[PATCHSIZE * PATCHSIZE];
	float *patchDis = new float[PATCHSIZE * PATCHSIZE];
	float *weight = new float[PATCHSIZE * PATCHSIZE];
	float *dis = new float[PATCHSIZE * PATCHSIZE];

	float w1[25];
	float pd[25];
	float d[25];
	Vec3f c[25];
	int length = 0;
	//debugShowOffset("before", offset);
	//waitKey();

	if(odd){
		for(int i = 0; i < PATCHSIZE; i++){
			for(int j = 0; j < PATCHSIZE; j++){
				int current_row = pixel_row - PATCHRADIUS + i;
				int current_col = pixel_col - PATCHRADIUS + j;
				int source_row = offset.at<cv::Vec3f>(current_row, current_col).val[1];
				int source_col = offset.at<cv::Vec3f>(current_row, current_col).val[0];

				if(current_row < PATCHRADIUS || current_row > target.rows -PATCHRADIUS - 1
				|| current_col < PATCHRADIUS || current_col > target.cols -PATCHRADIUS - 1)
					continue;
				if(current_row == pixel_row && current_col == pixel_col)
					int a = 1;
				propagationHole(source, target, offset, current_row, current_col, odd);
				//Propagation(target, source, offset, current_row, current_col, odd);
				randomSearchHole(source, target, offset, current_row, current_col);
				
				source_row = offset.at<cv::Vec3f>(current_row, current_col).val[1];
				source_col = offset.at<cv::Vec3f>(current_row, current_col).val[0];

				//float hole = offset.at<Vec3f>(source_row, source_col)[2];
				Mat t = getPatch(current_row, current_col, target);
				Mat s = getPatch(source_row, source_col, source);

				patchDis[length] = computeDistanceWithHole(t, s, offset, source_row - PATCHRADIUS, source_col - PATCHRADIUS);
				center[length] = s.at<cv::Vec3f>(PATCHSIZE - 1 - i, PATCHSIZE - 1 - j);
				dis[length] = sqrt((current_row - source_row) * (current_row - source_row) * 1.0 + (current_col - source_col) * (current_col - source_col) * 1.0) / (source.rows + source.cols);
				length++;
			}
		}
		//for(int i = 0; i < length; i++){
		//	weight[i] = exp(-patchDis[i]) * exp(-dis[i]);
		//}
	}
	else{
		for(int i = PATCHSIZE - 1; i >= 0; i--){
			for(int j = PATCHSIZE - 1; j >= 0; j--){
				int current_row = pixel_row - PATCHRADIUS + i;
				int current_col = pixel_col - PATCHRADIUS + j;

				if(current_row < PATCHRADIUS || current_row > target.rows -PATCHRADIUS - 1
				|| current_col < PATCHRADIUS || current_col > target.cols -PATCHRADIUS - 1)
					continue;
				propagationHole(source, target, offset, current_row, current_col, odd);
				//Propagation(target, source, offset, current_row, current_col, odd);
				randomSearchHole(source, target, offset, current_row, current_col);

				int source_row = offset.at<cv::Vec3f>(current_row, current_col).val[1];
				int source_col = offset.at<cv::Vec3f>(current_row, current_col).val[0];
				Mat t = getPatch(current_row, current_col, target);
				Mat s = getPatch(source_row, source_col, source);

				//patchDis[length] = computeDistance(t, s);
				patchDis[length] = computeDistanceWithHole(t, s, offset, source_row - PATCHRADIUS, source_col - PATCHRADIUS);
				center[length] = s.at<cv::Vec3f>(PATCHSIZE - 1 - i, PATCHSIZE - 1 - j);
				dis[length] = sqrt((current_row - source_row) * (current_row - source_row) * 1.0 + (current_col - source_col) * (current_col - source_col) * 1.0) / (source.rows + source.cols);
				length++;
			}
		}
		//for(int i = 0; i < length; i++){
		//	weight[i] = exp(-patchDis[i]) * exp(-dis[i]);
		//}
	}
	float max_pd = patchDis[0];
	float max_d = dis[0];

	
	for(int i = 1; i < length; i++){
		if(patchDis[i] > max_pd)
			max_pd = patchDis[i];

		if(dis[i] > max_d)
			max_d = dis[i];
		
	}
	for(int i = 0; i < length; i++){
		d[i] = dis[i];
		pd[i] = patchDis[i];
		c[i] = center[i];
	}
	for(int i = 0; i < length; i++){
		//if(max_d > 0)
		//	dis[i] /= max_d;
		if(max_pd > 0)
			patchDis[i] /= max_pd;
		weight[i] = 1.0 - exp(-dis[i]);// + exp(-patchDis[i]);

		d[i] = dis[i];
		pd[i] = patchDis[i];
		w1[i] = weight[i];
	}
	return getMeanHolePixel(weight, center, length, odd);
}

Vec3f MyPatchMatch::getMeanHolePixel(float* weight, Vec3f* pixels, int length, bool odd){
	Vec3b center;
	if(odd){
		center = pixels[0];
	}
	else{
		center = pixels[length];
	}
	int length1 = length;

	float b = 0, g = 0, r = 0;
	for(int i = 0; i < length; i++){
		b += pixels[i][0];
		g += pixels[i][1];
		r += pixels[i][2];
	}
	b = b / length;
	g = g / length;
	r = r / length;

	float delta = 0;
	for(int i = 0; i < length; i++){
		delta += (pixels[i][0] - b) * (pixels[i][0] - b) + (pixels[i][1] - g) * (pixels[i][1] - g)
			      + (pixels[i][2] - r) * (pixels[i][2] - r);
	}



	Vec3f *center1 = pixels;
	while(true){
		float *weight2 = new float[length1];
		Vec3f *center2 = new Vec3f[length1];
		int length2 = 0;

		for(int i = 0; i < length1; i++){
			if((center1[i][0] - center[0]) * (center1[i][0] - center[0])
				+(center1[i][1] - center[1]) * (center1[i][1] - center[1])
				+(center1[i][2] - center[2]) * (center1[i][2] - center[2]) < delta){
				weight2[length2] = weight[i];
				center2[length2] = center1[i];
				length2++;
			}
		}
		if(length2 == 0){
			delete center2;
			delete weight2;
			break;
		}

		b = 0;
		g = 0;
		r = 0;

		for(int i = 0; i < length2; i++){
			b += center2[i][0] - center[0];
			g += center2[i][1] - center[1];
			r += center2[i][2] - center[2];
		}
		b /= length2;
		g /= length2;
		r /= length2;

		Vec3f newCenter = Vec3b(b + center[0], g + center[1], r + center[2]);

		delete center1;
		delete weight;
		center1 = center2;
		weight = weight2;
		length1 = length2;

		if((newCenter[0] - center[0]) * (newCenter[0] - center[0])
		   + (newCenter[0] - center[0]) * (newCenter[0] - center[0])
		   + (newCenter[0] - center[0]) * (newCenter[0] - center[0]) < 1){
			break;
		}
		else{
			center = newCenter;
		}
	}

	w = 0;
	b = 0;
	g = 0;
	r = 0;

	for (int i = 0; i< length1; i++)
	{
		w += weight[i];
		b += ((int)center1[i][0]) * weight[i];
		g += ((int)center1[i][1]) * weight[i];
		r += ((int)center1[i][2]) * weight[i];
	}

	return Vec3f((b / w), (g / w), (r / w));
}

void MyPatchMatch::generateHole(Mat& target, Mat source, Mat offset, bool odd){
	if(odd){
		for(int row = 0; row < target.rows; row++){
			for(int col = 0; col < target.cols; col++){
				if(offset.at<cv::Vec3f>(row, col).val[2] == 0){
					//debugShowOffset("before", offset);
		
					//waitKey();

					//debugImage(target, "asd");
					Vec3f a = target.at<cv::Vec3f>(row, col);
					Vec3f result = generateHolePixel(target, source, offset, row, col, odd);
					
 					target.at<cv::Vec3f>(row, col) = result;
					//debugImage(target, "asdasdasd");
					//debugShowOffset("off", offset);
					//cvWaitKey();
				}
				else{
					//Vec3f a = target.at<cv::Vec3f>(row, col);
					//Vec3f result = target.at<cv::Vec3f>(row, col);
					//int b = 1;

				}
			}
		}
	}
	else{
		for(int row = target.rows - 1; row >= 0; row--){
			for(int col = target.cols - 1; col >= 0; col--){
				if(offset.at<cv::Vec3f>(row, col).val[2] == 0){
					//debugImage(target, "asd");
					Vec3f a = target.at<cv::Vec3f>(row, col);
					Vec3f result = generateHolePixel(target, source, offset, row, col, odd);
					
 					target.at<cv::Vec3f>(row, col) = result;
					//debugImage(target, "asdasdasd");
					//debugShowOffset("off", offset);
					//cvWaitKey();
				}
				else{
					//Vec3f a = target.at<cv::Vec3f>(row, col);
					//Vec3f result = target.at<cv::Vec3f>(row, col);
					//float x = a[0];
					//int b = 1;
				}
			}
		}
	}
	//debugShowOffset("off", offset);
	//cvWaitKey();
}

void MyPatchMatch::vote2(Mat source, Mat offset, Mat &result, Mat mask){
	Mat record(source.rows, source.cols, CV_32FC1, Scalar::all(0));
	Mat target(source.rows, source.cols, CV_32FC3, Scalar::all(0));

	for(int row = 0; row < target.rows; row++){
		for(int col = 0; col < target.cols; col++){	
			Mat recordPatch = getPatch(row, col, record);
			recordPatch +=1;

			int sourceRow = offset.at<Vec3f>(row, col).val[1];
			int sourceCol = offset.at<Vec3f>(row, col).val[0];

			Mat targetPatch = getPatch(row, col, target);
			Mat sourcePatch = getPatch(sourceRow, sourceCol, source);
			for(int y = 0; y < PATCHSIZE; y++){
				for(int x = 0; x < PATCHSIZE; x++){
					targetPatch.at<Vec3f>(y, x).val[0] += sourcePatch.at<Vec3f>(y, x).val[0];
					targetPatch.at<Vec3f>(y, x).val[1] += sourcePatch.at<Vec3f>(y, x).val[1];
					targetPatch.at<Vec3f>(y, x).val[2] += sourcePatch.at<Vec3f>(y, x).val[2];
				}
			}
		}
	}

	std::vector<Mat> lab(3);
	split(target,lab);
	lab[0] /= record;
	lab[1] /= record;
	lab[2] /= record;
	merge(lab,target); 
	
	//imshow("record", record);
	target.copyTo(result);
}





//////////////////////////////////////////////new copy method/////////////////////////////////////////////////////////////

void MyPatchMatch::imageInpaintCopy(Mat& SourceImageRGB, Mat TargetImageRGB){
	Mat inpaintMask[20];
	Mat Target[20];
	Mat Source[20];
	Mat Distance[20];
	Mat Offset = Mat(SourceImageRGB.size(), OFFSET_MAT_TYPE);
	Offset.setTo(0);
	Offset.at<OFFSET_DATA_TYPE>(0,0)[0] = 0;
	this->patch_radius = 2;
	int pyramidLevel = 0;
	
	Mat source = SourceImageRGB.clone();
	Source[0] = source.clone();

	inpaintMask[0] = Mat(SourceImageRGB.size(), CV_8UC3);
	inpaintMask[0].setTo(0);

	Distance[0] = Mat(SourceImageRGB.size(), CV_16SC1);
	Distance[0].setTo(0);
	Distance[0].at<unsigned short>(0,0) = 65536;

	initialCopyOffset(Offset, inpaintMask[0]);

	
	//cvWaitKey();
	Mat target = TargetImageRGB.clone();
	numberOfScales = 4;
	while (source.rows > 2 && source.cols > 2 && pyramidLevel < numberOfScales)
	{
		if(getHoleCount(inpaintMask[pyramidLevel]) == 0){
			pyramidLevel--;
			break;
		}
		pyramidLevel++;
		pyrDown(source, source);
		pyrDown(target, target);

		Mat newOff;
		pyDownOffset(newOff, Offset,inpaintMask[pyramidLevel - 1], inpaintMask[pyramidLevel]);
		Offset = newOff.clone();

		pyrDown(Distance[pyramidLevel - 1], Distance[pyramidLevel]);
		Distance[pyramidLevel].setTo(0);

		Source[pyramidLevel] = source.clone();

	}

	for(int i = 0; i < pyramidLevel - 1; i++){
		inpaintMask[i].setTo(0);
	}
	
	Mat currentMask;
	Mat currentDistance;
	
	//image pyramid 
	for(int level = pyramidLevel; level >= 1; level--){
		source = Source[level].clone();
		currentMask = inpaintMask[level].clone();
		currentDistance = Distance[level].clone();

		Mat needUpdateNNF = currentMask.clone();
		currentMask.setTo(0);

		if(level == pyramidLevel){
			conputeImageDistance(source, target, currentMask, Offset, currentDistance);
		}
		else{
			
			Mat newOff = Mat(source.rows, source.cols, OFFSET_MAT_TYPE);
			expandOffsetCopy(Offset, newOff);
			
			pyUpOffsetCopy(source, target, currentMask, Offset, newOff, currentDistance);
			Offset = newOff.clone();
		}

		Mat newTarget;
		int loopTimes = 2;//min(2 * level, 1);
		int passTimes = 2;//min(level, 4);
		for(int loop = 1; loop <= loopTimes; loop++){

			for(int pass = 0; pass < passTimes; pass++){
				for(int row = 0; row < target.rows; row++){
					for(int col = 0; col < target.cols; col++){
						if(needUpdateNNF.at<Vec3b>(row, col)[0] == 0)
							updatePixelNNF(source, target, Offset, currentMask, row, col, true, currentDistance);
					}
				}
				for(int row = target.rows - 1; row >= 0; row--){
					for(int col = target.cols - 1; col >= 0; col--){
						if(needUpdateNNF.at<Vec3b>(row, col)[0] == 0)
							updatePixelNNF(source, target, Offset, currentMask, row, col, false, currentDistance);
					}
				}
			}
			//needUpdateNNF.setTo(0);
			
			Mat newSource;
			bool upscale = false;
			if(loop == loopTimes){
				newSource = Source[level - 1].clone();
				resize(target, newTarget, newSource.size());
				currentMask = inpaintMask[level - 1];
				upscale = true;
			}
			else{
				newSource = Source[level].clone();
				newTarget = target.clone();
				upscale = false;
			}			
			
			vote(newSource, newTarget, Offset, currentMask, upscale, currentDistance);
			target = newTarget.clone();
		}
		target = newTarget.clone();
		showTempImage(target, SourceImageRGB.size());
		cvWaitKey(33);
	}
	
	//imshow("image", target);
	target.copyTo(SourceImageRGB);
	//cvWaitKey(33);
}

void MyPatchMatch::initialCopyOffset(Mat &offset, Mat &mask){
	int Offset = 0;
	//int staticUpLeftCornerX = newTopLeftX + Offset;
	//int staticUpLeftCornerY = newTopLeftY;
	
	Rect rect = getImageCopyMaskRect();
	if(newTopLeftY + 5 + rect.height < offset.rows)
		newTopLeftY += 5;
	for(int row = 0; row < offset.rows; row++){
		for(int col = 0; col < offset.cols; col++){
			if((col >= newTopLeftX + Offset && col <  newTopLeftX - Offset + rect.width)
				&& (row >= newTopLeftY + Offset && row <  newTopLeftY - Offset + rect.height)
				&& (ImageCopyMask[(row + rect.y - newTopLeftY) * offset.cols + col + rect.x - newTopLeftX] > 0)
				)
			{
				//if(ImageCopyMask[(row + rect.y - newTopLeftY) * offset.cols + col + rect.x - newTopLeftX] > 0)
				{
						offset.at<OFFSET_DATA_TYPE>(row, col).val[0] = col + rect.x - newTopLeftX;
						offset.at<OFFSET_DATA_TYPE>(row, col).val[1] = row + rect.y - newTopLeftY;
						offset.at<OFFSET_DATA_TYPE>(row, col).val[2] = 0;
						mask.at<Vec3b>(row, col)[0] = 255;
				}
			}
			//if(ImageCopyMask[row * offset.cols + col > 0]){
			//	offset.at<Vec3f>(row, col).val[0] = col + rect.x - newTopLeftX;
			//	offset.at<Vec3f>(row, col).val[1] = row + rect.y - newTopLeftY;
			//	offset.at<Vec3f>(row, col).val[2] = 255;
			//}
			else{
				int randomX = rand() % (offset.cols - 1);
				int randomY = rand() % (offset.rows - 1);
				offset.at<OFFSET_DATA_TYPE>(row, col).val[0] = randomX;
				offset.at<OFFSET_DATA_TYPE>(row, col).val[1] = randomY;
				offset.at<OFFSET_DATA_TYPE>(row, col).val[2] = 0;
				mask.at<Vec3b>(row, col)[0] = 0;
			}
			mask.at<Vec3b>(row, col)[1] = 0;
			mask.at<Vec3b>(row, col)[2] = 0;
		}
	}
}

void MyPatchMatch::conputeImageDistance(Mat source, Mat target, Mat mask, Mat &Offset, Mat& currentDistance){
	currentDistance = Mat(source.rows, source.cols, CV_16SC1);
	for(int row = 0; row < Offset.rows; row++){
		for(int col = 0; col < Offset.cols; col++){
			currentDistance.at<unsigned short>(row, col) = computeDistance(target, row, col, source, Offset.at<OFFSET_DATA_TYPE>(row, col)[1], Offset.at<OFFSET_DATA_TYPE>(row, col)[0], mask);

			//int iter = 0;
			//int times = 20;
			////find patch that is out of hole
			//while(currentDistance.at<unsigned short>(row, col) == MaxValue && iter < times) {
			//	Offset.at<OFFSET_DATA_TYPE>(row, col)[0] = rand() % Offset.cols;
			//	Offset.at<OFFSET_DATA_TYPE>(row, col)[1] = rand() % Offset.rows;
			//	currentDistance.at<unsigned short>(row, col) = computeDistance(target, row, col, source, Offset.at<OFFSET_DATA_TYPE>(row, col)[1], Offset.at<OFFSET_DATA_TYPE>(row, col)[0], mask);
			//	iter++;
			//}
			
		}
	}
}

void MyPatchMatch::pyUpOffsetCopy(Mat source, Mat target, Mat mask, Mat& oldOff, Mat& newOff, Mat &currentDistance){
	//initial offset
	//for(int row = 0; row < newOff.rows; row++) {
	//	for(int col = 0; col < newOff.cols; col++) {
	//		int xlow = min(col / 2, oldOff.cols - 1);
	//		int ylow = min(row / 2, oldOff.rows - 1);
	//		newOff.at<OFFSET_DATA_TYPE>(row, col)[0] = oldOff.at<OFFSET_DATA_TYPE>(ylow, xlow)[0] * 2;
	//		newOff.at<OFFSET_DATA_TYPE>(row, col)[1] = oldOff.at<OFFSET_DATA_TYPE>(ylow, xlow)[1] * 2;
	//		newOff.at<OFFSET_DATA_TYPE>(row, col)[2] = 0;
	//		currentDistance.at<unsigned short>(row, col) = 0;
	//	}
	//}
	currentDistance = Mat(source.rows, source.cols, CV_16SC1);
	//initial distance
	for(int row = 0;row < newOff.rows; row++) {
		for(int col = 0; col < newOff.cols; col++) {
			currentDistance.at<unsigned short>(row, col) = computeDistance(target, row, col, source, newOff.at<OFFSET_DATA_TYPE>(row, col)[1], newOff.at<OFFSET_DATA_TYPE>(row, col)[0], mask);
		}
	}
}

void MyPatchMatch::pyDownOffset(Mat& newOff, Mat& oldOff){
	newOff = Mat((oldOff.rows + 1) / 2, (oldOff.cols + 1) / 2, OFFSET_MAT_TYPE);
	//initial offset
	for(int row = 0; row < newOff.rows; row++) {
		for(int col = 0; col < newOff.cols; col++) {
			int xup = min(col * 2, oldOff.cols - 1);
			int yup = min(row * 2, oldOff.rows - 1);

			newOff.at<OFFSET_DATA_TYPE>(row, col)[0] = oldOff.at<OFFSET_DATA_TYPE>(yup, xup)[0] / 2;
			newOff.at<OFFSET_DATA_TYPE>(row, col)[1] = oldOff.at<OFFSET_DATA_TYPE>(yup, xup)[1] / 2;
			newOff.at<OFFSET_DATA_TYPE>(row, col)[2] = 0;
		}
	}
}

void MyPatchMatch::pyDownOffset(Mat& newOffsetMap, Mat& offsetMap, Mat& oldMask, Mat& newMask){
	newOffsetMap = Mat((offsetMap.rows + 1) / 2, (offsetMap.cols + 1) / 2, OFFSET_MAT_TYPE);
	newMask = Mat((oldMask.rows + 1) / 2, (oldMask.cols + 1) / 2, CV_8UC3);

	for(int row = 0; row < newOffsetMap.rows; row++){
			int b = row * 2;
			if (b >= offsetMap.rows - 1)
				b = offsetMap.rows - 2;
			for (int col = 0; col < newOffsetMap.cols ;col++)
			{
				int a = col * 2;
				if (a >= offsetMap.cols - 1)
					a = offsetMap.cols - 2;


				newOffsetMap.at<OFFSET_DATA_TYPE>(row, col).val[0] = offsetMap.at<OFFSET_DATA_TYPE>(b,a).val[0] / 2;
				newOffsetMap.at<OFFSET_DATA_TYPE>(row, col).val[1] = offsetMap.at<OFFSET_DATA_TYPE>(b,a).val[1] / 2;
				newOffsetMap.at<OFFSET_DATA_TYPE>(row, col).val[2] = 0;

				if ((oldMask.at<Vec3b>(b, a).val[0] == 255) || (oldMask.at<Vec3b>(b + 1, a).val[0] == 255)
					|| (oldMask.at<Vec3b>(b, a + 1).val[0] == 255) || (oldMask.at<Vec3b>(b + 1 , a + 1).val[0] == 255))
				{
					if (oldMask.at<Vec3b>(b + 1, a).val[0] == 255)
					{
						newOffsetMap.at<OFFSET_DATA_TYPE>(row, col).val[0] = offsetMap.at<OFFSET_DATA_TYPE>(b + 1, a).val[0] / 2;
						newOffsetMap.at<OFFSET_DATA_TYPE>(row, col).val[1] = offsetMap.at<OFFSET_DATA_TYPE>(b + 1, a).val[1] / 2;
					}
					else if (oldMask.at<Vec3b>(b, a + 1).val[0] == 255)
					{
						newOffsetMap.at<OFFSET_DATA_TYPE>(row, col).val[0] = offsetMap.at<OFFSET_DATA_TYPE>(b, a + 1).val[0] / 2;
						newOffsetMap.at<OFFSET_DATA_TYPE>(row, col).val[1] = offsetMap.at<OFFSET_DATA_TYPE>(b, a + 1).val[1] / 2;
					}
					else if (oldMask.at<Vec3b>(b + 1, a + 1).val[0] == 255)
					{
						newOffsetMap.at<OFFSET_DATA_TYPE>(row, col).val[0] = offsetMap.at<OFFSET_DATA_TYPE>(b + 1, a + 1).val[0] / 2;
						newOffsetMap.at<OFFSET_DATA_TYPE>(row, col).val[1] = offsetMap.at<OFFSET_DATA_TYPE>(b + 1, a + 1).val[1] / 2;
					}
					newMask.at<Vec3b>(row, col).val[0] = 255;
				}
				else
					newMask.at<Vec3b>(row, col).val[0] = 0;
			}
		}
}

void MyPatchMatch::expandOffsetCopy(Mat oldOffset, Mat& newOffset){
	//for(int row = 0; row < oldOffset.rows; row++){
	//	for(int col = 0; col < oldOffset.cols; col++){
	//		float disRow = (oldOffset.at<OFFSET_DATA_TYPE>(row, col).val[1] - row) * 2;
	//		float disCol = (oldOffset.at<OFFSET_DATA_TYPE>(row, col).val[0] - col) * 2;
	//		newOffset.at<OFFSET_DATA_TYPE>(row * 2, col * 2)[0] = col * 2 + disCol;
	//		newOffset.at<OFFSET_DATA_TYPE>(row * 2, col * 2)[1] = row * 2 + disRow;;
	//		newOffset.at<OFFSET_DATA_TYPE>(row * 2, col * 2)[2] = oldOffset.at<OFFSET_DATA_TYPE>(row, col)[2];
	//		newOffset.at<OFFSET_DATA_TYPE>(row * 2 + 1, col * 2)[0] = col * 2 + disCol;
	//		newOffset.at<OFFSET_DATA_TYPE>(row * 2 + 1, col * 2)[1] = row * 2 + 1 + disRow;;
	//		newOffset.at<OFFSET_DATA_TYPE>(row * 2 + 1, col * 2)[2] = oldOffset.at<OFFSET_DATA_TYPE>(row, col)[2];
	//		newOffset.at<OFFSET_DATA_TYPE>(row * 2, col * 2 + 1)[0] = col * 2 + 1 + disCol;
	//		newOffset.at<OFFSET_DATA_TYPE>(row * 2, col * 2 + 1)[1] = row * 2 + disRow;;
	//		newOffset.at<OFFSET_DATA_TYPE>(row * 2, col * 2 + 1)[2] = oldOffset.at<OFFSET_DATA_TYPE>(row, col)[2];
	//		newOffset.at<OFFSET_DATA_TYPE>(row * 2 + 1, col * 2 + 1)[0] = col * 2 + 1 + disCol;
	//		newOffset.at<OFFSET_DATA_TYPE>(row * 2 + 1, col * 2 + 1)[1] = row * 2 + 1 + disRow;;
	//		newOffset.at<OFFSET_DATA_TYPE>(row * 2 + 1, col * 2 + 1)[2] = oldOffset.at<OFFSET_DATA_TYPE>(row, col)[2];
	//	}
	//}

	for(int row = 0; row < newOffset.rows; row++){
		for(int col = 0; col < newOffset.cols; col++){
			int disRow = (oldOffset.at<OFFSET_DATA_TYPE>(row / 2, col / 2).val[1] - row / 2) * 2;
			int disCol = (oldOffset.at<OFFSET_DATA_TYPE>(row / 2, col / 2).val[0] - col / 2) * 2;

			newOffset.at<OFFSET_DATA_TYPE>(row, col)[0] = col + disCol;
			newOffset.at<OFFSET_DATA_TYPE>(row, col)[1] = row + disRow;;
			newOffset.at<OFFSET_DATA_TYPE>(row, col)[2] = oldOffset.at<OFFSET_DATA_TYPE>(row / 2, col / 2)[2];
		}
	}
}

void MyPatchMatch::pyDownSourceCopy(Mat& source, Mat& mask, Mat& newMask){
	int newW = source.cols/2, newH=source.rows/2;
	
	//guass filter
	int kernel[] = {1, 5, 10, 10, 5, 1}; 
	int kernel_length = 6;

	Mat newSource = Mat(newH, newW, CV_8UC3);
	newMask = Mat(newH, newW, CV_8UC3);
	newSource.setTo(0);
	newMask.setTo(0);

	int row = -2;
	for(int new_row = 0; new_row < newSource.rows; new_row++){
		row += 2;
		int col = -2;
		for(int new_col=0; new_col < newSource.cols; new_col++){		
			col += 2;
			int all_count = 0;
			int mask_count = 0;
			int total_weight = 0;

			int b = 0, g = 0, r = 0;	
			for(int dy = 0; dy < kernel_length; dy++) {
				int kernel_row = row + dy - 2;
				if (kernel_row < 0 || kernel_row >= source.rows) 
					continue;
				for(int dx = 0; dx < kernel_length; dx++) {
					int kernel_col = col + dx - 2;
					if (kernel_col < 0 || kernel_col >= source.cols) 
						continue;
						
					all_count++;

					if (mask.at<Vec3b>(kernel_row, kernel_col)[0] == 255){
						mask_count++;
					}
						
					int weight = kernel[dx] * kernel[dy];
					b += weight * source.at<Vec3b>(kernel_row, kernel_col)[0];
					g += weight * source.at<Vec3b>(kernel_row, kernel_col)[1];
					r += weight * source.at<Vec3b>(kernel_row, kernel_col)[2];
					total_weight += weight;
					
				}
			}
				
			if (total_weight > 0) {
				newSource.at<Vec3b>(new_row, new_col)[0] = (int)(b * 1.0 / total_weight + 0.5);
				newSource.at<Vec3b>(new_row, new_col)[1] = (int)(g * 1.0 / total_weight + 0.5);
				newSource.at<Vec3b>(new_row, new_col)[2] = (int)(r * 1.0 / total_weight + 0.5);
			} 
						
			if (mask_count > 0.75 * all_count){
				newMask.at<Vec3b>(new_row, new_col)[0] = 255;
			}
			else{
				newMask.at<Vec3b>(new_row, new_col)[0] = 0;
			}

			newMask.at<Vec3b>(new_row, new_col)[1] = 0;
			newMask.at<Vec3b>(new_row, new_col)[2] = 0;
		}
	}
	source = newSource.clone();
}





//////////////////////////////////////////////new inpaint method/////////////////////////////////////////////////////////////
void MyPatchMatch::imageInpaint(Mat& SourceImageRGB, Mat TargetImageRGB){
	Mat inpaintMask[20];
	Mat Target[20];
	Mat Source[20];
	Mat Distance[20];
	Mat Offset = Mat(SourceImageRGB.size(), OFFSET_MAT_TYPE);
	Offset.setTo(0);
	Offset.at<OFFSET_DATA_TYPE>(0,0)[0] = 0;
	this->patch_radius = 2;
	int pyramidLevel = 0;
	
	Mat source = SourceImageRGB.clone();
	Source[0] = source.clone();

	inpaintMask[0] = Mat(SourceImageRGB.size(), CV_8UC3);
	inpaintMask[0].setTo(0);

	Distance[0] = Mat(SourceImageRGB.size(), CV_16SC1);
	Distance[0].setTo(0);
	//Distance[0].at<unsigned short>(0,0) = 65536;

	for(int row = 0; row < SourceImageRGB.rows; row++){
		for(int col = 0; col < SourceImageRGB.cols; col++){
			if(ImageInpaintMask[row * SourceImageRGB.cols + col] > 0)
				inpaintMask[0].at<cv::Vec3b>(row, col).val[0] = 255;
			else 
				inpaintMask[0].at<cv::Vec3b>(row, col).val[0] = 0;
			inpaintMask[0].at<cv::Vec3b>(row, col).val[1] = 0;
			inpaintMask[0].at<cv::Vec3b>(row, col).val[2] = 0;
		}
	}
	//imshow("show", inpaintMask[0]);
	//cvWaitKey();
	while (source.rows > 2 && source.cols > 2 && pyramidLevel < 20)
	{
		if(getHoleCount(inpaintMask[pyramidLevel]) == 0)
			break;
		pyramidLevel++;
		pyDownSource(source, inpaintMask[pyramidLevel-1], inpaintMask[pyramidLevel]);

		Distance[pyramidLevel] = Mat(Distance[pyramidLevel-1].rows / 2, Distance[pyramidLevel-1].cols / 2, CV_16SC1);
		Distance[pyramidLevel].setTo(0);

		Source[pyramidLevel] = source.clone();
		//imshow("show1", source);
		//cvWaitKey();
	}



	Mat target = Source[pyramidLevel].clone();
	
	Mat currentMask;
	Mat currentDistance;
	
	//image pyramid 
	for(int level = pyramidLevel; level >= 1; level--){
		source = Source[level].clone();
		currentMask = inpaintMask[level].clone();
		currentDistance = Distance[level].clone();

		if(level == pyramidLevel){
			initialOffset(source, target, currentMask, Offset, currentDistance);
		}
		else{
			Mat newOff = Mat(currentMask.size(), OFFSET_MAT_TYPE);
			pyUpOffset(source, target, currentMask, Offset, newOff, currentDistance);
			Offset = newOff.clone();
		}

		Mat newTarget;
		int loopTimes = max(2 * level, 6);
		int passTimes = max(level, 4);
		for(int loop = 1; loop <= loopTimes; loop++){
			//for each pixel use propagation and random search in two direction
			for(int pass = 0; pass < passTimes; pass++){
				for(int row = 0; row < target.rows; row++){
					for(int col = 0; col < target.cols; col++){
						if(currentDistance.at<unsigned short>(row, col) > 0)
							updatePixelNNF(source, target, Offset, currentMask, row, col, true, currentDistance);
					}
				}
				for(int row = target.rows - 1; row >= 0; row--){
					for(int col = target.cols - 1; col >= 0; col--){
						if(currentDistance.at<unsigned short>(row, col) > 0)
							updatePixelNNF(source, target, Offset, currentMask, row, col, false, currentDistance);
					}
				}
			}
			Mat newSource;
			bool upscale = false;
			if(level >= 1 && loop == loopTimes){
				newSource = Source[level - 1];
				resize(target, newTarget, newSource.size());
				currentMask = inpaintMask[level - 1];
				upscale = true;
			}
			else{
				newSource = Source[level];
				newTarget = target.clone();
				upscale = false;
			}

			vote(newSource, newTarget, Offset, currentMask, upscale, currentDistance);
			target = newTarget.clone();
		}
		target = newTarget.clone();
		showTempImage(target, SourceImageRGB.size());

	}
	target.copyTo(TargetImageRGB);
}

void MyPatchMatch::imageInpaintWithRestrict(Mat& SourceImageRGB, Mat TargetImageRGB, char* holeInRestrict){
	Mat inpaintMask[20];
	Mat Target[20];
	Mat Source[20];
	Mat Distance[20];
	Mat RestrictRegion[20];
	Mat Offset = Mat(SourceImageRGB.size(), OFFSET_MAT_TYPE);
	Offset.setTo(0);
	Offset.at<OFFSET_DATA_TYPE>(0,0)[0] = 0;
	this->patch_radius = 2;
	int pyramidLevel = 0;
	
	Mat source = SourceImageRGB.clone();
	Source[0] = source.clone();

	inpaintMask[0] = Mat(SourceImageRGB.size(), CV_8UC3);
	inpaintMask[0].setTo(0);

	Distance[0] = Mat(SourceImageRGB.size(), CV_16SC1);
	Distance[0].setTo(0);
	//Distance[0].at<unsigned short>(0,0) = 65536;

	RestrictRegion[0] = Mat(SourceImageRGB.size(), CV_8UC3);
	RestrictRegion[0].setTo(0);


	for(int row = 0; row < SourceImageRGB.rows; row++){
		for(int col = 0; col < SourceImageRGB.cols; col++){
			//只考虑在restrict区域内部的hole
			if(holeInRestrict[row * SourceImageRGB.cols + col] > 0)
				inpaintMask[0].at<cv::Vec3b>(row, col).val[0] = 255;
			else 
				inpaintMask[0].at<cv::Vec3b>(row, col).val[0] = 0;
			inpaintMask[0].at<cv::Vec3b>(row, col).val[1] = 0;
			inpaintMask[0].at<cv::Vec3b>(row, col).val[2] = 0;

			//初始化RestrictRegion
			if(InpaintRestrictMask[row * SourceImageRGB.cols + col] > 0)
				RestrictRegion[0].at<cv::Vec3b>(row, col).val[0] = 255;
			else 
				RestrictRegion[0].at<cv::Vec3b>(row, col).val[0] = 0;
			RestrictRegion[0].at<cv::Vec3b>(row, col).val[1] = 0;
			RestrictRegion[0].at<cv::Vec3b>(row, col).val[2] = 0;
		}
	}

	while (source.rows > 2 && source.cols > 2 && pyramidLevel < 20)
	{
		if(getHoleCount(inpaintMask[pyramidLevel]) == 0 || getHoleCount(RestrictRegion[pyramidLevel]) == 0)
			break;
		pyramidLevel++;
		Mat tempSource = source.clone();
		pyDownSource(source, inpaintMask[pyramidLevel-1], inpaintMask[pyramidLevel]);
		pyDownSource(tempSource, RestrictRegion[pyramidLevel-1], RestrictRegion[pyramidLevel]);

		Distance[pyramidLevel] = Mat(Distance[pyramidLevel-1].rows / 2, Distance[pyramidLevel-1].cols / 2, CV_16SC1);
		Distance[pyramidLevel].setTo(0);

		Source[pyramidLevel] = source.clone();
		//imshow("show1", source);
		//cvWaitKey();
	}



	Mat target = Source[pyramidLevel].clone();
	
	Mat currentMask;
	Mat currentDistance;
	
	//image pyramid 
	for(int level = pyramidLevel; level >= 1; level--){
		source = Source[level].clone();
		currentMask = inpaintMask[level].clone();
		currentDistance = Distance[level].clone();
		currentRestrict = RestrictRegion[level].clone();
		getRestrictPos();

		if(level == pyramidLevel){
			initialOffset(source, target, currentMask, Offset, currentDistance);
		}
		else{
			Mat newOff = Mat(currentMask.size(), OFFSET_MAT_TYPE);
			pyUpOffset(source, target, currentMask, Offset, newOff, currentDistance);
			Offset = newOff.clone();
		}

		
		Mat newTarget;
		int loopTimes = max(2 * level, 3);
		int passTimes = max(level, 3);

		for(int loop = 1; loop <= loopTimes; loop++){
			//for each pixel use propgation and randomsearch in two direction
			for(int pass = 0; pass < passTimes; pass++){
				for(int row = 0; row < target.rows; row++){
					for(int col = 0; col < target.cols; col++){
						if(currentDistance.at<unsigned short>(row, col) > 0)
							updatePixelNNF(source, target, Offset, currentMask, row, col, true, currentDistance);
					}
				}
				for(int row = target.rows - 1; row >= 0; row--){
					for(int col = target.cols - 1; col >= 0; col--){
						if(currentDistance.at<unsigned short>(row, col) > 0)
							updatePixelNNF(source, target, Offset, currentMask, row, col, false, currentDistance);
					}
				}
			}
			
			Mat newSource;
			bool upscale = false;
			if(level >= 1 && loop == loopTimes){
				newSource = Source[level - 1];
				resize(target, newTarget, newSource.size());
				currentMask = inpaintMask[level - 1];
				upscale = true;
			}
			else{
				newSource = Source[level];
				newTarget = target.clone();
				upscale = false;
			}

			vote(newSource, newTarget, Offset, currentMask, upscale, currentDistance);
			target = newTarget.clone();
		}
		target = newTarget.clone();
		showTempImage(target, SourceImageRGB.size());
	}
	
	imshow("image", target);
	target.copyTo(TargetImageRGB);
	cvWaitKey(33);
}

int MyPatchMatch::computeDistance(Mat source, int source_row, int source_col, Mat target, int target_row, int target_col, Mat mask){
	long distance = 0;
	long weight_sum = 0;
	long penalty = 655350;
	int radius = this->patch_radius;
	for(int dy =- radius; dy <= radius; dy++) {
		for(int dx =- radius; dx <= radius; dx++) {
			weight_sum += penalty;
			
			int s_row = source_row + dy;
			int s_col = source_col + dx;
			if (s_row < 0 || s_row >= source.rows){
				distance += penalty;
				continue;
			}

			if (s_col < 0 || s_col >= source.cols){
				distance += penalty;
				continue;
			}
			
			int t_row= target_row + dy;
			int t_col = target_col + dx;
			if (t_row < 0 || t_row >= target.rows){
				distance += penalty;
				continue;
			}
			if (t_col < 0 || t_col >= target.cols){
				distance += penalty;
				continue;
			}
			
			//penalty for hole in source
			if (mask.at<Vec3b>(t_row, t_col)[0] == 255) {
				distance += penalty;
				continue;
			}
			if(ImageInpaintModeWhenWork == INPAINT_RESTRICT){
				if (mask.at<Vec3b>(s_row, s_col)[0] == 255) {
					//判断target是否在restrict内部，若不在，则惩罚
					if(currentRestrict.at<Vec3b>(t_row, t_col)[0] == 0){
						distance += penalty;
						continue;
					}
				}
			}

			//compute ssd
			long ssd = 0;
			for(int channel = 0; channel < 3; channel++){
				int weight = (channel == 0)? 1: (channel == 1)? 6: 3; //B:G:R = 1:6:3
				//int s = source.at<Vec3b>(s_row, s_col)[channel];
				//int t = target.at<Vec3b>(t_row, t_col)[channel];
				//double dis = (s - t) * (s - t);
				double dis = pow( source.at<Vec3b>(s_row, s_col)[channel] - target.at<Vec3b>(t_row, t_col)[channel] , 2); // Value 
				ssd += weight * dis;
			}
			distance += ssd;
		}
	}

	return (int)(MaxValue * (distance * 1.0 / weight_sum));
}

void MyPatchMatch::updatePixelNNF(Mat source, Mat target, Mat offset, Mat mask, int row, int col, bool odd, Mat& currentDistance){
		int x, y, dis;
		int xchange = 1, ychange = 1;
		if(!odd){
			xchange = -1;
			ychange = -1;
		}
		//propagation
		if (col - xchange > 0 && col - xchange < offset.cols) {
			x = offset.at<OFFSET_DATA_TYPE>(row, col - xchange)[0] + xchange;
			y = offset.at<OFFSET_DATA_TYPE>(row, col - ychange)[1];
			dis = computeDistance(target, row, col, source, y, x, mask);
			if (dis < currentDistance.at<unsigned short>(row, col)) {
				offset.at<OFFSET_DATA_TYPE>(row, col)[0] = x;
				offset.at<OFFSET_DATA_TYPE>(row, col)[1] = y;
				currentDistance.at<unsigned short>(row,col) = dis;
			}
		}
		
		if (row - ychange > 0 && row - ychange < offset.rows) {
			x = offset.at<OFFSET_DATA_TYPE>(row - xchange, col)[0];
			y = offset.at<OFFSET_DATA_TYPE>(row - ychange, col)[1] + ychange;
			dis = computeDistance(target, row, col, source, y, x, mask);
			if (dis < currentDistance.at<unsigned short>(row, col)) {
				offset.at<OFFSET_DATA_TYPE>(row, col)[0] = x;
				offset.at<OFFSET_DATA_TYPE>(row, col)[1] = y;
				currentDistance.at<unsigned short>(row,col) = dis;
			}
		}
		
		//random search
		if(ImageInpaintModeWhenWork == INPAINT_RESTRICT){
			int window_size;
			if(mask.at<Vec3b>(row, col)[0] == 255){
				window_size = max(restrictRight - restrictLeft, restrictUp - restrictDown);			
			}
			else{
				window_size = max(offset.rows, offset.cols);
			}
			int origin_x = offset.at<OFFSET_DATA_TYPE>(row, col)[0];
			int origin_y = offset.at<OFFSET_DATA_TYPE>(row, col)[1];
			while(window_size > 0) {
				x = origin_x + rand() % (2 * window_size) - window_size;
				x = max(0, min(offset.cols - 1, x));

				y = origin_y + rand() % (2 * window_size) - window_size;
				y = max(0, min(offset.rows - 1, y));

				dis = computeDistance(target, row, col, source, y, x, mask);
				if (dis < currentDistance.at<unsigned short>(row, col)) {
					offset.at<OFFSET_DATA_TYPE>(row, col)[0] = x;
					offset.at<OFFSET_DATA_TYPE>(row, col)[1] = y;
					currentDistance.at<unsigned short>(row, col) = dis;
				}
				window_size /= 2;
			}
		}

		else{
			int window_size = max(offset.rows, offset.cols);
			int origin_x = offset.at<OFFSET_DATA_TYPE>(row, col)[0];
			int origin_y = offset.at<OFFSET_DATA_TYPE>(row, col)[1];
			while(window_size > 0) {
				x = origin_x + rand() % (2 * window_size) - window_size;
				x = max(0, min(offset.cols - 1, x));

				y = origin_y + rand() % (2 * window_size) - window_size;
				y = max(0, min(offset.rows - 1, y));

				dis = computeDistance(target, row, col, source, y, x, mask);
				if (dis < currentDistance.at<unsigned short>(row, col)) {
					offset.at<OFFSET_DATA_TYPE>(row, col)[0] = x;
					offset.at<OFFSET_DATA_TYPE>(row, col)[1] = y;
					currentDistance.at<unsigned short>(row, col) = dis;
				}
				window_size /= 2;
			}
		}
}

void MyPatchMatch::pyDownSource(Mat& source, Mat& mask, Mat& newMask){
	int newW = source.cols/2, newH=source.rows/2;
	
	//guass filter
	int kernel[] = {1, 5, 10, 10, 5, 1}; 
	int kernel_length = 6;

	Mat newSource = Mat(newH, newW, CV_8UC3);
	newMask = Mat(newH, newW, CV_8UC3);
	newSource.setTo(0);
	newMask.setTo(0);

	int row = -2;
	for(int new_row = 0; new_row < newSource.rows; new_row++){
		row += 2;
		int col = -2;
		for(int new_col=0; new_col < newSource.cols; new_col++){		
			col += 2;
			int all_count = 0;
			int mask_count = 0;
			int total_weight = 0;

			int b = 0, g = 0, r = 0;	
			for(int dy = 0; dy < kernel_length; dy++) {
				int kernel_row = row + dy - 2;
				if (kernel_row < 0 || kernel_row >= source.rows) 
					continue;
				for(int dx = 0; dx < kernel_length; dx++) {
					int kernel_col = col + dx - 2;
					if (kernel_col < 0 || kernel_col >= source.cols) 
						continue;
						
					all_count++;

					if (mask.at<Vec3b>(kernel_row, kernel_col)[0] == 255){
						mask_count++;
						continue;
					}
						
					int weight = kernel[dx] * kernel[dy];
					b += weight * source.at<Vec3b>(kernel_row, kernel_col)[0];
					g += weight * source.at<Vec3b>(kernel_row, kernel_col)[1];
					r += weight * source.at<Vec3b>(kernel_row, kernel_col)[2];
					total_weight += weight;
					
				}
			}
				
			if (total_weight > 0) {
				newSource.at<Vec3b>(new_row, new_col)[0] = (int)(b * 1.0 / total_weight + 0.5);
				newSource.at<Vec3b>(new_row, new_col)[1] = (int)(g * 1.0 / total_weight + 0.5);
				newSource.at<Vec3b>(new_row, new_col)[2] = (int)(r * 1.0 / total_weight + 0.5);
			} 
						
			if (mask_count > 0.75 * all_count){
				newMask.at<Vec3b>(new_row, new_col)[0] = 255;
			}
			else{
				newMask.at<Vec3b>(new_row, new_col)[0] = 0;
			}

			newMask.at<Vec3b>(new_row, new_col)[1] = 0;
			newMask.at<Vec3b>(new_row, new_col)[2] = 0;
		}
	}
	source = newSource.clone();
}

void MyPatchMatch::pyUpTarget(Mat& oldTarget, Mat& newTarget){
	resize(oldTarget, newTarget, newTarget.size());
}

void MyPatchMatch::vote(Mat source, Mat target, Mat& offset, Mat mask, bool upscale, Mat& distance){
	int radius = this->patch_radius;
	if (upscale) radius *= 2;
	double histo[3][256];

	for(int row = 0; row < source.rows; row++) {
		for(int col = 0; col < source.cols; col++) {

			//clear histograms
			for(int i = 0; i < 3; i++){
				for(int j = 0; j < 256; j++){
					histo[i][j] = 0;
				}
			}
			double weight_sum = 0;
			int s_row, s_col; 
			double weight;
			for(int dy = -radius; dy <= radius; dy++) {
				for(int dx = -radius; dx <= radius; dx++) {				
					int t_col = col + dx, t_row = row + dy;				
					if (!upscale) {
						if (t_row < 0 || t_row >= offset.rows) 
							continue;
						if (t_col < 0 || t_col >= offset.cols) 
							continue;
				
						s_col = offset.at<OFFSET_DATA_TYPE>(t_row, t_col)[0];
						s_row = offset.at<OFFSET_DATA_TYPE>(t_row, t_col)[1];
						weight = similarity[distance.at<unsigned short>(t_row, t_col)];
					} else {
						if (t_row < 0 || t_row >= 2 * offset.rows) 
							continue;
						if (t_col < 0 || t_col >= 2 * offset.cols) 
							continue;
						
						s_col = 2 * offset.at<OFFSET_DATA_TYPE>(t_row / 2, t_col / 2)[0] + (t_col % 2);
						s_row = 2 * offset.at<OFFSET_DATA_TYPE>(t_row / 2, t_col / 2)[1] + (t_row % 2);
						weight = similarity[distance.at<unsigned short>(t_row / 2, t_col / 2)];
					}
						
					//get the pixel in target that responce to current pixel 
					int c_row = s_row - dy, c_col = s_col - dx;
					if (c_row < 0 || c_row >= source.rows) 
						continue;
					if (c_col < 0 || c_col >= source.cols) 
						continue;					
					
					if (mask.at<Vec3b>(c_row, c_col)[0] == 255) 
						continue;
					int b = source.at<Vec3b>(c_row, c_col)[0];
					int g = source.at<Vec3b>(c_row, c_col)[1];
					int r = source.at<Vec3b>(c_row, c_col)[2];

					histo[0][b] += weight;
					histo[1][g] += weight;
					histo[2][r] += weight;
					weight_sum += weight;
				}
			}

			if (weight_sum < 1) continue;
				

			//use median value in histo to get current pixel 
			double lb = 0.30 * weight_sum;  
			double hb = 0.70 * weight_sum; 

			for(int channel = 0; channel < 3; channel++) {
				double integration = 0, median_histovalue = 0, median_histoweight = 0;
				for(int i = 0; i < 256; i++) {
					integration += histo[channel][i];
					if (integration < lb) 
						continue;
					median_histovalue += histo[channel][i] * i; 
					median_histoweight += histo[channel][i];
					if (integration > hb) 
						break;
				}
				int value = (int)(median_histovalue /median_histoweight);

				target.at<Vec3b>(row, col)[channel] = value;
			}
		}
	}
}

int MyPatchMatch::getHoleCount(Mat& mask){
	int count = 0;
	for(int row = 0; row < mask.rows; row++){
		for(int col = 0; col < mask.cols; col++){
			if(mask.at<Vec3b>(row, col)[0] == 255)
				count++;
		}
	}
	return count;
}

void MyPatchMatch::initialOffset(Mat source, Mat target, Mat mask, Mat &Offset, Mat &currentDistance){
	Offset = Mat(source.size(), OFFSET_MAT_TYPE);
	currentDistance = Mat(source.rows, source.cols, CV_16SC1);
	//initial offset
	for(int row = 0; row < Offset.rows; row++){
		for(int col = 0; col < Offset.cols; col++){
			Offset.at<OFFSET_DATA_TYPE>(row, col)[0] = rand() % Offset.cols;
			Offset.at<OFFSET_DATA_TYPE>(row, col)[1] = rand() % Offset.rows;
			Offset.at<OFFSET_DATA_TYPE>(row, col)[2] = 0;
			int a = 1;
			currentDistance.at<unsigned short>(row, col) = 0;
		}
	}

	//initial distance
	for(int row = 0; row < Offset.rows; row++){
		for(int col = 0; col < Offset.cols; col++){
			currentDistance.at<unsigned short>(row, col) = computeDistance(target, row, col, source, Offset.at<OFFSET_DATA_TYPE>(row, col)[1], Offset.at<OFFSET_DATA_TYPE>(row, col)[0], mask);

			int iter = 0;
			int times = 20;
			//find patch that is out of hole
			while(currentDistance.at<unsigned short>(row, col) == MaxValue && iter < times) {
				Offset.at<OFFSET_DATA_TYPE>(row, col)[0] = rand() % Offset.cols;
				Offset.at<OFFSET_DATA_TYPE>(row, col)[1] = rand() % Offset.rows;
				currentDistance.at<unsigned short>(row, col) = computeDistance(target, row, col, source, Offset.at<OFFSET_DATA_TYPE>(row, col)[1], Offset.at<OFFSET_DATA_TYPE>(row, col)[0], mask);
				iter++;
			}
			
		}
	}
}

void MyPatchMatch::pyUpOffset(Mat source, Mat target, Mat mask, Mat& oldOff, Mat& newOff, Mat &currentDistance){
	currentDistance = Mat(source.rows, source.cols, CV_16SC1);
	//initial offset
	for(int row = 0; row < newOff.rows; row++) {
		for(int col = 0; col < newOff.cols; col++) {
			int xlow = min(col / 2, oldOff.cols - 1);
			int ylow = min(row / 2, oldOff.rows - 1);

			newOff.at<OFFSET_DATA_TYPE>(row, col)[0] = oldOff.at<OFFSET_DATA_TYPE>(ylow, xlow)[0] * 2;
			newOff.at<OFFSET_DATA_TYPE>(row, col)[1] = oldOff.at<OFFSET_DATA_TYPE>(ylow, xlow)[1] * 2;
			newOff.at<OFFSET_DATA_TYPE>(row, col)[2] = 0;
			currentDistance.at<unsigned short>(row, col) = 0;
		}
	}

	//initial distance
	for(int row = 0;row < newOff.rows; row++) {
		for(int col = 0; col < newOff.cols; col++) {
			currentDistance.at<unsigned short>(row, col) = computeDistance(target, row, col, source, newOff.at<OFFSET_DATA_TYPE>(row, col)[1], newOff.at<OFFSET_DATA_TYPE>(row, col)[0], mask);

			int iter = 0;
			int times = 20;
			//find patch that is out of hole
			if(ImageInpaintModeWhenWork == INPAINT_HOLE){
				while(currentDistance.at<unsigned short>(row, col) == MaxValue && iter < times) {
					newOff.at<OFFSET_DATA_TYPE>(row, col)[0] = rand() % newOff.cols;
					newOff.at<OFFSET_DATA_TYPE>(row, col)[1] = rand() % newOff.rows;
					currentDistance.at<unsigned short>(row, col) = computeDistance(target, row, col, source, newOff.at<OFFSET_DATA_TYPE>(row, col)[1], newOff.at<OFFSET_DATA_TYPE>(row, col)[0], mask);
					iter++;
				}
			}
			//make hole's responce patch in restrict
			else if(ImageInpaintModeWhenWork == INPAINT_RESTRICT){
				while(currentDistance.at<unsigned short>(row, col) == MaxValue && iter < times) {
					if(mask.at<Vec3b>(row, col)[0] == 255){
						newOff.at<OFFSET_DATA_TYPE>(row, col)[0] = rand() % (restrictRight - restrictLeft) + restrictLeft;
						newOff.at<OFFSET_DATA_TYPE>(row, col)[1] = rand() % (restrictUp - restrictDown) + restrictDown;
					}
					else{
						newOff.at<OFFSET_DATA_TYPE>(row, col)[0] = rand() % newOff.cols;
						newOff.at<OFFSET_DATA_TYPE>(row, col)[1] = rand() % newOff.rows;
					}
					currentDistance.at<unsigned short>(row, col) = computeDistance(target, row, col, source, newOff.at<OFFSET_DATA_TYPE>(row, col)[1], newOff.at<OFFSET_DATA_TYPE>(row, col)[0], mask);
					iter++;
				}
			}
		}
	}
}

void MyPatchMatch::initialSimilarity(){
	double s_zero = 0.999;
	double t_halfmax = 0.10;
	double x = (s_zero - 0.5) * 2;
	double invtanh = 0.5 * log((1 + x) / (1 - x));
	double coef = invtanh / t_halfmax;
	similarity = new double[65536];
	for(int i = 0; i < 65536; i++) {
		double t = i * 1.0 / 65536;
		similarity[i] = 0.5 - 0.5 * tanh(coef * (t - t_halfmax));
	}
}

void MyPatchMatch::deleteSimilarity(){
	if(similarity != NULL){
		delete similarity;
		similarity = NULL;
	}
}

void MyPatchMatch::debugoffsettxt(Mat Offset, Mat dis){
	//Mat off = Offset.clone();
	//char filename[] = "debug.txt";

	//ofstream fout;
	//fout.open(filename);
	//for(int row = 0; row < off.rows; row++){
	//	for(int col  = 0; col < off.cols; col++){
	//		fout<<off.at<OFFSET_DATA_TYPE>(row, col)[0]-col<<"_"<<off.at<OFFSET_DATA_TYPE>(row, col)[1]-row<<"_"<<dis.at<unsigned short>(row, col)<<" ";
	//	}
	//	fout<<endl;
	//}
	//fout.close();
}

void MyPatchMatch::addToImageInpaintMask(int ax, int ay, int rows, int cols){
	int radius = ImageInpaintRadius;
	int centerX = ax;
	int centerY = ay;
	imageCols = cols;
	imageRows = rows;
	for(int row = -radius; row < radius + 1; row++){
		for(int col = -radius; col < radius + 1; col++){
			if(centerX + col < 0 || centerY + row < 0 || centerX + col >= cols || centerY + row >= rows)
				continue;
			if(row * row + col * col < radius * radius){	
				ImageInpaintMask[(centerY + row) * cols + centerX + col] = 1;
			}
		}
	}
}

void MyPatchMatch::addToImageInpaintRestrictMask(int ax, int ay, int rows, int cols){
	int radius = ImageInpaintRestrictRadius;
	int centerX = ax;
	int centerY = ay;
	imageCols = cols;
	imageRows = rows;
	for(int row = -radius; row < radius + 1; row++){
		for(int col = -radius; col < radius + 1; col++){
			if(centerX + col < 0 || centerY + row < 0 || centerX + col >= cols || centerY + row >= rows)
				continue;
			if(row * row + col * col < radius * radius){	
				InpaintRestrictMask[(centerY + row) * cols + centerX + col] = 1;
			}
		}
	}
}

void MyPatchMatch::showImageInpaintRestrictMaskImage(Mat &image){
	Mat temp;
	image.copyTo(temp);
	for(int row = 0; row < image.rows; row++){
		for(int col = 0; col < image.cols; col++){
			if(InpaintRestrictMask[row * image.cols + col] > 0){
				//image.at<cv::Vec3b>(row, col).val[1] = (image.at<cv::Vec3b>(row, col).val[1] + 255) / 2;
				image.at<cv::Vec3b>(row, col).val[0] = 0;
				image.at<cv::Vec3b>(row, col).val[1] = 0;
				image.at<cv::Vec3b>(row, col).val[2] = (image.at<cv::Vec3b>(row, col).val[2] + 255) / 2;
			}
		}	
	}
}

void MyPatchMatch::showTempImage(Mat image, Size originSize){
	Mat temp = image.clone();
	resize(temp, temp, originSize);
	imshow("image", temp);
	waitKey(33);
}

void MyPatchMatch::getRestrictPos(){
	restrictLeft = currentRestrict.cols;
	restrictRight = 0;
	restrictUp = currentRestrict.rows;
	restrictDown = 0;

	for(int row = 0; row < currentRestrict.rows; row++){
		for(int col = 0; col < currentRestrict.cols; col++){
			if(InpaintRestrictMask[row * currentRestrict.cols + col] > 0){
				if(col > restrictRight)
					restrictRight = col;
				if(col < restrictLeft)
					restrictLeft = col;
				if(row > restrictUp)
					restrictUp = row;
				if(row < restrictDown)
					restrictDown = row;
			}
		}	
	}
}

void MyPatchMatch::zeroInpaintMask(int length, bool newImage){
	if(newImage){
		if(ImageInpaintMask != NULL)
			delete ImageInpaintMask;
		ImageInpaintMask = new char[length];

		if(InpaintRestrictMask != NULL)
			delete InpaintRestrictMask;
		InpaintRestrictMask = new char[length];
		
	}

	for(int i = 0; i < length; i++){
		ImageInpaintMask[i] = 0;
		InpaintRestrictMask[i] = 0;
	}
}

void MyPatchMatch::initialImageInpaintOffset(Mat &offset, Mat mask){
	//init offset 
	int radius = 1;
	for(int row = 0; row < mask.rows; row++){
		for(int col = 0; col < mask.cols; col++){
			if(mask.at<cv::Vec3f>(row, col).val[0] == HOLE){
				
				int randomX = rand() % (offset.cols - 1);
				int randomY = rand() % (offset.rows - 1);

				while(mask.at<cv::Vec3f>(randomY, randomX).val[0] != SOURCE ){
					randomX = rand() % (offset.cols - 1);
					randomY = rand() % (offset.rows - 1);
				}
				offset.at<Vec3f>(row, col).val[0] = randomX;
				offset.at<Vec3f>(row, col).val[1] = randomY;
				offset.at<Vec3f>(row, col).val[2] = 0;
			}
			else{
				offset.at<Vec3f>(row, col).val[0] = col;
				offset.at<Vec3f>(row, col).val[1] = row;
				offset.at<Vec3f>(row, col).val[2] = 255;
			}
		}
	}
}

void MyPatchMatch::showImageInpaintMaskImage(Mat &image){
	Mat temp;
	image.copyTo(temp);
	for(int row = 0; row < image.rows; row++){
		for(int col = 0; col < image.cols; col++){
			if(ImageInpaintMask[row * image.cols + col] > 0){
				image.at<cv::Vec3b>(row, col).val[0] = 0;
				image.at<cv::Vec3b>(row, col).val[1] = 0;
				image.at<cv::Vec3b>(row, col).val[2] = 0;
			}
		}	
	}
}

void MyPatchMatch::addCircleToImageInpaint(cv::Mat &image, int ax, int ay){
	int radius;
	int b, g, r;

	switch(ImageInpaintMode){
	case INPAINT_HOLE:
		b = 0;
		g = 0;
		r = 0;
		radius = ImageInpaintRadius;
		break;
	case INPAINT_RESTRICT:
		b = 0;
		g = 0;
		r = 150;
		radius = ImageInpaintRestrictRadius;
		break;
	}
	
	int centerX = ax;
	int centerY = ay;
	for(int row = -radius; row < radius + 1; row++){
		for(int col = -radius; col < radius + 1; col++){
			if(centerX + col < 0 || centerY + row < 0 || centerX + col >= image.cols || centerY + row >= image.rows)
				continue;
			if(row * row + col * col < radius * radius){	
				image.at<cv::Vec3b>(centerY + row, centerX + col).val[0] = b;
				image.at<cv::Vec3b>(centerY + row, centerX + col).val[1] = g;
				image.at<cv::Vec3b>(centerY + row, centerX + col).val[2] = r;
			}
		}
	}
}

void MyPatchMatch::erodeMask(int rows, int cols){
	int radius = erodeRadius;
	char *copy = new char[rows * cols];
	for(int i = 0; i < rows * cols; i++){
		copy[i] = ImageInpaintMask[i];
	}

	for(int row = 0; row < rows; row++){
		for(int col = 0; col < cols; col++){
			if(copy[row * cols + col] == 1){
				int rMin = row - radius > 0 ? row - radius : 0;
				int rMax = row + radius < rows - 1? row + radius : rows - 1;

				int cMin = col - radius > 0 ? col - radius : 0;
				int cMax = col + radius < cols - 1? col + radius : cols - 1;
		
				for(int i = rMin; i <= rMax; i++){
					for(int j = cMin; j <= cMax; j++){
						ImageInpaintMask[i * cols + j] = 1;
					}
				}
			}
		}
	}

	delete copy;
}





//////////////////////////////////////////interact/////////////////////////////////////////////
//inpaint
void MyPatchMatch::dealWithImageInpaintMouseButtonDown(int ax, int ay, Mat &image){
	switch (ImageInpaintMode)
	{
	case INPAINT_HOLE:
		addToImageInpaintMask(ax, ay, image.rows, image.cols);
		//showImageInpaintMaskImage(image, ax, ay);
		break;
	case INPAINT_RESTRICT:
		addToImageInpaintRestrictMask(ax, ay, image.rows, image.cols);
		//showImageInpaintRestrictMaskImage(image, ax, ay);
		break;
	default:
		break;
	}
}

void MyPatchMatch::dealWithImageInpaintMouseButtonUp(int ax, int ay, Mat &image){
	switch (ImageInpaintMode)
	{
	case INPAINT_HOLE:
		//showImageInpaintMaskImage(image, ax, ay);
		break;
	case INPAINT_RESTRICT:
		//showImageInpaintRestrictMaskImage(image, ax, ay);
		break;
	default:
		break;
	}
	//mouseDown = false;
}

void MyPatchMatch::dealWithImageInpaintMouseMoveEvent(int ax, int ay, Mat &image){
	//mouseDown = true;
	switch (ImageInpaintMode)
	{
	case INPAINT_HOLE:
		addToImageInpaintMask(ax, ay, image.rows, image.cols);
		//showImageInpaintMaskImage(image, ax, ay);
		break;
	case INPAINT_RESTRICT:
		addToImageInpaintRestrictMask(ax, ay, image.rows, image.cols);
		//showImageInpaintRestrictMaskImage(image, ax, ay);
		break;

	default:
		break;
	}
}


//copy
void MyPatchMatch::dealWithImageCopyMouseMoveEvent(int ax, int ay, Mat &image){
	Mat temp;
	image.copyTo(temp);
 	switch (ImageCopyMode)
	{
	case ADDTARGET:
		addToImageCopyMask(ax, ay, image.rows, image.cols);
		//show mask on image
		showImageCopyMaskImage(image);
		break;
	case MOVE:
		if(selectImageCopyPatch){
			getAndShowImageCopyMovedImage(ax, ay, image, false);
		}

		break;
	}
}

void MyPatchMatch::dealWithImageCopyMouseMoveButtonDown(int ax, int ay, Mat &image){
	Mat temp;
	image.copyTo(temp);
	switch (ImageCopyMode)
	{
	case ADDTARGET:
		addToImageCopyMask(ax, ay, image.rows, image.cols);
		//show mask on image
		showImageCopyMaskImage(image);
		break;
	case MOVE:
		Rect targetRect = getImageCopyMaskRect();
		if(targetRect.height > 0 || targetRect.width > 0){
			int topleftx = targetRect.x;
			int toplefty = targetRect.y;
			int downrightx = topleftx + targetRect.width - 1;
			int downrighty = toplefty + targetRect.height - 1;
			if(ax >= topleftx && ax <= downrightx && ay >= toplefty && ay <= downrighty){
				selectImageCopyPatch = true;
				oldTopLeftX = topleftx;
				oldTopLeftY = toplefty;
				mouseDownX = ax;
				mouseDownY = ay;
			}
		}
		break;
	}
}

bool MyPatchMatch::dealWithImageCopyMouseMoveButtonUp(int ax, int ay, Mat &image){
	bool finished = false;
	switch (ImageCopyMode)
	{
	case ADDTARGET:
		addToImageCopyMask(ax, ay, image.rows, image.cols);
		//show mask on image
		break;
	case MOVE:
		if(selectImageCopyPatch){
			ImageCopyMoveStart = true;
			getAndShowImageCopyMovedImage(ax, ay, image, true);
			//ImageCopyMode = ADDTARGET;
			ImageCopyMoveStart = false;
			finished = true;
			zeroImageCopyMask(image.cols * image.rows, false);
		}
		selectImageCopyPatch = false;
		break;

	default:
		break;
	}
	return finished;
}






//////////////////////////////////////////for debug/////////////////////////////////////////////
bool MyPatchMatch::debugShowOffset(string name,  Mat Offset){
	Mat offset = Mat(Offset.rows, Offset.cols, CV_8UC3);;
	//Offset.copyTo(offset);
	for(int row = 0; row < offset.rows; row++){
		for(int col = 0; col < offset.cols; col++){
			offset.at<Vec3b>(row, col)[2] = (Offset.at<OFFSET_DATA_TYPE>(row, col)[0] - col) % 256;
			offset.at<Vec3b>(row, col)[1] = (Offset.at<OFFSET_DATA_TYPE>(row, col)[1] - row) % 256;;
			offset.at<Vec3b>(row, col)[0] = 0;
		}
	}
	imshow(name, offset);
	return true;
}

void MyPatchMatch::debugImage(Mat &Image, char * name){
	Mat tempImage = Image.clone();
	cvtColor(tempImage, tempImage, CV_Lab2RGB);
	tempImage.convertTo(tempImage,CV_8UC3, 255, 0);
	
	imshow(name, tempImage);
}

void MyPatchMatch::debugMat(Mat mat){
	for(int row = 0; row < mat.rows; row++){
		for(int col = 0; col < mat.cols; col++){
			mat.at<cv::Vec3f>(row, col) = Vec3f(col,row,0);
		}
	}
}

bool MyPatchMatch::debugOffset(Mat& offset){
	for(int row = 0; row < offset.rows; row++){
		for(int col = 0; col < offset.cols; col++){
			int x = offset.at<Vec3f>(row, col).val[0];
			int y = offset.at<Vec3f>(row, col).val[1];
			if(offset.at<Vec3f>(row, col).val[0] < 0 || offset.at<Vec3f>(row, col).val[1] < 0)
				return false;
		}
	}
	return true;
}


void MyPatchMatch::initOpencl(){
	//get platform numbers
	cl_uint num;
	cl_int err;
	err = clGetPlatformIDs(0, 0, &num);
 
	//get all platforms
	platforms.resize(num);
	err = clGetPlatformIDs(num, &platforms[0], &num);

	//get device num
	err=clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_ALL,0,0,&num);
	device_id.resize(num);
	//get all device
	err=clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_ALL,num,&device_id[0],&num);

	//set property with certain platform
	cl_context_properties prop[] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platforms[0]), 0 };
	contextCl = clCreateContextFromType(prop, CL_DEVICE_TYPE_ALL, NULL, NULL, &err);

	clQueue = clCreateCommandQueue(contextCl, device_id[0], 0, 0);

	//buildProgram("openclShader\\updatePixel.cl", "updatePixel", propagationKernel, propagationProgram);
	buildProgram("openclShader\\jumpFlooding.cl", "jumpFlooding", jumpFloodingKernel, jumpFloodingProgram);

	buildProgram("openclShader\\jumpFloodingCopy.cl", "jumpFloodingCopy", jumpFloodingCopyKernel, jumpFloodingCopyProgram);

	buildProgram("openclShader\\jumpFloodingRestrict.cl", "jumpFloodingRestrict", jumpFloodingRestrictKernel, jumpFloodingRestrictProgram);
	//read source and build program and kernel
	//cl_kernel adder;
	//cl_program program;
	//buildProgram("openclShader\\test.cl", "adder", adder, program);

	//cl_kernel adder2;
	//cl_program program2;
	//buildProgram("openclShader\\test1.cl", "test2", adder2, program2);

	////create mem
	//int DATA_SIZE = 100;
	//std::vector<ushort> a(DATA_SIZE), b(DATA_SIZE);
	//for(int i = 0; i < DATA_SIZE; i++) {
	//	a[i] = i;
	//	b[i] = i;
	//}

	////copy to video mem
	//cl_mem cl_a = clCreateBuffer(contextCl, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_ushort) * DATA_SIZE, &a[0], NULL);
	//cl_mem cl_b = clCreateBuffer(contextCl, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_ushort) * DATA_SIZE, &b[0], NULL);

	////result
	//cl_mem cl_res = clCreateBuffer(contextCl, CL_MEM_WRITE_ONLY, sizeof(cl_ushort) * DATA_SIZE, NULL, NULL);

	////set parameters
	//clSetKernelArg(adder, 0, sizeof(cl_mem), &cl_a);
	//clSetKernelArg(adder, 1, sizeof(cl_mem), &cl_b);
	//clSetKernelArg(adder, 2, sizeof(cl_mem), &cl_res);

	//size_t worksize(100);
	//err = clEnqueueNDRangeKernel(clQueue, adder, 1, 0, &worksize, 0, 0, 0, 0);
	//std::vector<ushort> res(DATA_SIZE);
	//err = clEnqueueReadBuffer(clQueue, cl_res, CL_TRUE, 0, sizeof(ushort) * DATA_SIZE, &res[0], 0, 0, 0);

	////set parameters
	//clSetKernelArg(adder2, 0, sizeof(cl_mem), &cl_a);
	//clSetKernelArg(adder2, 1, sizeof(cl_mem), &cl_b);
	//clSetKernelArg(adder2, 2, sizeof(cl_mem), &cl_res);

	//err = clEnqueueNDRangeKernel(clQueue, adder2, 1, 0, &worksize, 0, 0, 0, 0);
	//err = clEnqueueReadBuffer(clQueue, cl_res, CL_TRUE, 0, sizeof(ushort) * DATA_SIZE, &res[0], 0, 0, 0);

	//clReleaseKernel(adder);
	//clReleaseProgram(program);
	//clReleaseMemObject(cl_a);
	//clReleaseMemObject(cl_b);
	//clReleaseMemObject(cl_res);

	////for(size_t i=0;i<num;i++){
	////	clReleaseCommandQueue(cqueue[i]);
	////}
	//clReleaseCommandQueue(clQueue);
	//clReleaseContext(contextCl);
}
	
void MyPatchMatch::destroyOpencl(){
	clReleaseKernel(jumpFloodingKernel);
	clReleaseProgram(jumpFloodingProgram);

	clReleaseKernel(jumpFloodingCopyKernel);
	clReleaseProgram(jumpFloodingCopyProgram);

	clReleaseKernel(jumpFloodingRestrictKernel);
	clReleaseProgram(jumpFloodingRestrictProgram);

	clReleaseCommandQueue(clQueue);
	clReleaseContext(contextCl);
}


cl_int MyPatchMatch::buildProgram(char* file_name, char* kernel_name, cl_kernel& current_kernel, cl_program &current_program){
	cl_int err;
	//read shader
	ifstream in(file_name, std::ios_base::end);

	in.seekg(0, std::ios_base::end);
	size_t length = in.tellg();
	in.seekg(0, std::ios_base::beg);
	
	// read program source
	std::vector<char> data(length + 1);
	in.read(&data[0], length);
	data[length] = 0;

	// create and build program 
	const char* source = &data[0];
	//build program
	current_program = clCreateProgramWithSource(contextCl, 1, &source, 0, &err);
	err = clBuildProgram(current_program, device_id.size(),  (cl_device_id*)device_id.data(), 0, 0, 0);
	if (err == CL_BUILD_PROGRAM_FAILURE) {
		// Determine the size of the log
		size_t log_size;
		clGetProgramBuildInfo(current_program, device_id[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

		// Allocate memory for the log
		char *log = (char *) malloc(log_size);

		// Get the log
		clGetProgramBuildInfo(current_program, device_id[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

		// Print the log
		printf("%s\n", log);
	}
	current_kernel = clCreateKernel(current_program, kernel_name, &err);

	return err;
}

void MyPatchMatch::imageInpaintCL_JF_TEST(Mat& SourceImageRGB, Mat TargetImageRGB){
	Mat inpaintMask[20];
	Mat Target[20];
	Mat Source[20];
	Mat Distance[20];
	Mat Offset = Mat(SourceImageRGB.size(), OFFSET_MAT_TYPE);
	Offset.setTo(0);
	Offset.at<OFFSET_DATA_TYPE>(0,0)[0] = 0;
	this->patch_radius = 2;
	int pyramidLevel = 0;
	
	Mat source = SourceImageRGB.clone();
	Source[0] = source.clone();

	inpaintMask[0] = Mat(SourceImageRGB.size(), CV_8UC3);
	inpaintMask[0].setTo(0);

	Distance[0] = Mat(SourceImageRGB.size(), CV_16SC1);
	Distance[0].setTo(0);
	Distance[0].at<unsigned short>(0,0) = 65536;

	for(int row = 0; row < SourceImageRGB.rows; row++){
		for(int col = 0; col < SourceImageRGB.cols; col++){
			//if(ImageInpaintMask[row * SourceImageRGB.cols + col] > 0)
			//	inpaintMask[0].at<cv::Vec3b>(row, col).val[0] = 255;
			//else 
				inpaintMask[0].at<cv::Vec3b>(row, col).val[0] = 0;
			inpaintMask[0].at<cv::Vec3b>(row, col).val[1] = 0;
			inpaintMask[0].at<cv::Vec3b>(row, col).val[2] = 0;
		}
	}
	imshow("showS", SourceImageRGB);
	imshow("showT", TargetImageRGB);
	cvWaitKey();
	//while (source.rows > 2 && source.cols > 2 && pyramidLevel < 20)
	//{
		//if(getHoleCount(inpaintMask[pyramidLevel]) == 0)
		//	break;
		pyramidLevel++;
		pyDownSource(source, inpaintMask[pyramidLevel-1], inpaintMask[pyramidLevel]);

		pyDownSource(TargetImageRGB, inpaintMask[pyramidLevel-1], inpaintMask[pyramidLevel]);


		Distance[pyramidLevel] = Mat(Distance[pyramidLevel-1].rows / 2, Distance[pyramidLevel-1].cols / 2, CV_16SC1);
		Distance[pyramidLevel].setTo(0);

		Source[pyramidLevel] = source.clone();
		//imshow("show1", source);
		//cvWaitKey();
	//}



	Mat target = TargetImageRGB.clone();
	
	Mat currentMask;
	Mat currentDistance;
	
	//image pyramid 
	//kNN
	int KNN = 5;
	Mat OffsetKNN[16];
	Mat currentDistanceKNN[16];
	for(int level = pyramidLevel; level >= 1; level--){
		source = Source[level].clone();
		currentMask = inpaintMask[level].clone();
		currentDistance = Distance[level].clone();
		
		if(level == pyramidLevel){
			//initialOffset(source, target, currentMask, Offset, currentDistance);

			for(int i = 0; i < KNN; i++){
				//currentDistanceKNN[i] = Distance[level].clone();
				initialOffset(source, target, currentMask, OffsetKNN[i], currentDistanceKNN[i]);
			}
			//for(int row = 0; row < source.rows; row++){
			//	for(int col = 0; col < source.cols; col++){
			//		for(int i = 0; i < KNN - 1; i++){
			//			for(int j = i + 1; j < KNN; j++){
			//				if(currentDistanceKNN[i].at<ushort>(row, col) > currentDistanceKNN[j].at<ushort>(row, col)){
			//					ushort c = currentDistanceKNN[i].at<ushort>(row, col);
			//					currentDistanceKNN[i].at<ushort>(row, col) = currentDistanceKNN[j].at<ushort>(row, col);
			//					currentDistanceKNN[j].at<ushort>(row, col) = c; 
			//					OFFSET_DATA_TYPE c1 = OffsetKNN[i].at<OFFSET_DATA_TYPE>(row, col);
			//					OffsetKNN[i].at<OFFSET_DATA_TYPE>(row, col) = OffsetKNN[j].at<OFFSET_DATA_TYPE>(row, col);
			//					OffsetKNN[j].at<OFFSET_DATA_TYPE>(row, col) = c1;
			//				}
			//			}
			//		}
			//	}
			//}
		}
		else{
			//Mat newOff = Mat(currentMask.size(), OFFSET_MAT_TYPE);
			//pyUpOffset(source, target, currentMask, Offset, newOff, currentDistance);
			//Offset = newOff.clone();

			for(int i = 0; i < KNN; i++){
				Mat newOff = Mat(currentMask.size(), OFFSET_MAT_TYPE);
				pyUpOffset(source, target, currentMask, OffsetKNN[i], newOff, currentDistanceKNN[i]);
				OffsetKNN[i] = newOff.clone();
			}
		}
		//debugShowOffset("asdasdasd",OffsetKNN[0]);
		//cvWaitKey(33);
		Mat newTarget;
		//int iterLoop = max(level * 2, 4);
		//int iterNNF = max(level, 3);
		int loopTimes = 1;//max(2 * level, 6);
		int passTimes = max(level, 4);
		for(int loop = 1; loop <= loopTimes; loop++){
			//target = source.clone();
			//jumpFlooding3(source, target, Offset, currentMask, currentDistance);
			
			//Mat OffsetKNN2[16], currentDistanceKNN2[16]; 
			//for(int i = 0; i < KNN; i++){
			//	OffsetKNN2[i] = OffsetKNN[i].clone();
			//	currentDistanceKNN2[i] = currentDistanceKNN[i].clone();
			//}
			//jumpFlooding(source, target, OffsetKNN2, currentMask, currentDistanceKNN2, KNN);
			jumpFlooding(source, target, OffsetKNN, currentMask, currentDistanceKNN, KNN);
			
			//debugShowOffset("asdasdasd",OffsetKNN[0]);
			//cvWaitKey(33);
			Mat newSource;
			bool upscale = false;
			if(level >= 1 && loop == loopTimes){
				newSource = Source[level - 1];
				resize(target, newTarget, newSource.size());
				currentMask = inpaintMask[level - 1];
				upscale = true;
			}
			else{
				newSource = Source[level];
				newTarget = target.clone();
				upscale = false;
			}

			vote(newSource, newTarget, OffsetKNN[0], currentMask, upscale, currentDistanceKNN[0]);
			target = newTarget.clone();
		}
		target = newTarget.clone();
		showTempImage(target, SourceImageRGB.size());
		cvWaitKey(33);
	}
	
	//imshow("image", target);
	target.copyTo(TargetImageRGB);
	debugShowOffset("asdasdadoffset", OffsetKNN[0]);
	//cvWaitKey(33);
}

void MyPatchMatch::imageInpaintCL_JF(Mat& SourceImageRGB, Mat TargetImageRGB){
	Mat inpaintMask[20];
	Mat Target[20];
	Mat Source[20];
	Mat Distance[20];
	Mat Offset = Mat(SourceImageRGB.size(), OFFSET_MAT_TYPE);
	Offset.setTo(0);
	Offset.at<OFFSET_DATA_TYPE>(0,0)[0] = 0;
	this->patch_radius = 2;
	int pyramidLevel = 0;
	
	Mat source = SourceImageRGB.clone();
	Source[0] = source.clone();

	inpaintMask[0] = Mat(SourceImageRGB.size(), CV_8UC3);
	inpaintMask[0].setTo(0);

	Distance[0] = Mat(SourceImageRGB.size(), CV_16SC1);
	Distance[0].setTo(0);
	Distance[0].at<unsigned short>(0,0) = 65536;

	for(int row = 0; row < SourceImageRGB.rows; row++){
		for(int col = 0; col < SourceImageRGB.cols; col++){
			if(ImageInpaintMask[row * SourceImageRGB.cols + col] > 0)
				inpaintMask[0].at<cv::Vec3b>(row, col).val[0] = 255;
			else 
				inpaintMask[0].at<cv::Vec3b>(row, col).val[0] = 0;
			inpaintMask[0].at<cv::Vec3b>(row, col).val[1] = 0;
			inpaintMask[0].at<cv::Vec3b>(row, col).val[2] = 0;
		}
	}
	while (source.rows > 2 && source.cols > 2 && pyramidLevel < 20)
	{
		if(getHoleCount(inpaintMask[pyramidLevel]) == 0)
			break;
		pyramidLevel++;
		pyDownSource(source, inpaintMask[pyramidLevel-1], inpaintMask[pyramidLevel]);

		Distance[pyramidLevel] = Mat(Distance[pyramidLevel-1].rows / 2, Distance[pyramidLevel-1].cols / 2, CV_16SC1);
		Distance[pyramidLevel].setTo(0);

		Source[pyramidLevel] = source.clone();
		//imshow("show1", source);
		//cvWaitKey();
	}



	Mat target = Source[pyramidLevel].clone();
	
	Mat currentMask;
	Mat currentDistance;
	
	//image pyramid 
	//kNN
	int KNN = 5;
	Mat OffsetKNN[16];
	Mat currentDistanceKNN[16];
	for(int level = pyramidLevel; level >= 1; level--){
		source = Source[level].clone();
		currentMask = inpaintMask[level].clone();
		currentDistance = Distance[level].clone();
		
		if(level == pyramidLevel){
			//initialOffset(source, target, currentMask, Offset, currentDistance);

			for(int i = 0; i < KNN; i++){
				//currentDistanceKNN[i] = Distance[level].clone();
				initialOffset(source, target, currentMask, OffsetKNN[i], currentDistanceKNN[i]);
			}
			//for(int row = 0; row < source.rows; row++){
			//	for(int col = 0; col < source.cols; col++){
			//		for(int i = 0; i < KNN - 1; i++){
			//			for(int j = i + 1; j < KNN; j++){
			//				if(currentDistanceKNN[i].at<ushort>(row, col) > currentDistanceKNN[j].at<ushort>(row, col)){
			//					ushort c = currentDistanceKNN[i].at<ushort>(row, col);
			//					currentDistanceKNN[i].at<ushort>(row, col) = currentDistanceKNN[j].at<ushort>(row, col);
			//					currentDistanceKNN[j].at<ushort>(row, col) = c; 
			//					OFFSET_DATA_TYPE c1 = OffsetKNN[i].at<OFFSET_DATA_TYPE>(row, col);
			//					OffsetKNN[i].at<OFFSET_DATA_TYPE>(row, col) = OffsetKNN[j].at<OFFSET_DATA_TYPE>(row, col);
			//					OffsetKNN[j].at<OFFSET_DATA_TYPE>(row, col) = c1;
			//				}
			//			}
			//		}
			//	}
			//}
		}
		else{
			for(int i = 0; i < KNN; i++){
				Mat newOff = Mat(currentMask.size(), OFFSET_MAT_TYPE);
				pyUpOffset(source, target, currentMask, OffsetKNN[i], newOff, currentDistanceKNN[i]);
				OffsetKNN[i] = newOff.clone();
			}
		}
		Mat newTarget;
		int loopTimes = max(2 * level, 6);
		int passTimes = max(level, 4);
		for(int loop = 1; loop <= loopTimes; loop++){
			jumpFlooding(source, target, OffsetKNN, currentMask, currentDistanceKNN, KNN);

			Mat newSource;
			bool upscale = false;
			if(level >= 1 && loop == loopTimes){
				newSource = Source[level - 1];
				resize(target, newTarget, newSource.size());
				currentMask = inpaintMask[level - 1];
				upscale = true;
			}
			else{
				newSource = Source[level];
				newTarget = target.clone();
				upscale = false;
			}

			vote(newSource, newTarget, OffsetKNN[0], currentMask, upscale, currentDistanceKNN[0]);
			target = newTarget.clone();
		}
		target = newTarget.clone();
		showTempImage(target, SourceImageRGB.size());
		cvWaitKey(33);
	}
	
	//imshow("image", target);
	target.copyTo(TargetImageRGB);
}

void MyPatchMatch::imageInpaintCopyCL_JF(Mat& SourceImageRGB, Mat TargetImageRGB){
	Mat inpaintMask[20];
	Mat Target[20];
	Mat Source[20];
	Mat Distance[20];
	Mat Offset = Mat(SourceImageRGB.size(), OFFSET_MAT_TYPE);
	Offset.setTo(0);
	Offset.at<OFFSET_DATA_TYPE>(0,0)[0] = 0;
	this->patch_radius = 2;
	int pyramidLevel = 0;
	
	Mat source = SourceImageRGB.clone();
	Source[0] = source.clone();

	inpaintMask[0] = Mat(SourceImageRGB.size(), CV_8UC3);
	inpaintMask[0].setTo(0);

	Distance[0] = Mat(SourceImageRGB.size(), CV_16SC1);
	Distance[0].setTo(0);
	//Distance[0].at<unsigned short>(0,0) = 65536;

	int KNN = 5;
	Mat OffsetKNN[16];
	Mat currentDistanceKNN[16];
	for(int i = 0; i < KNN; i++){
		OffsetKNN[i] = Mat(SourceImageRGB.size(), OFFSET_MAT_TYPE);
		initialCopyOffset(OffsetKNN[i], inpaintMask[0]);
		//currentDistanceKNN[i] = Mat(SourceImageRGB.size(), CV_16SC1);
		//currentDistanceKNN[i].setTo(0);
	}

	
	//cvWaitKey();
	Mat target = TargetImageRGB.clone();

	numberOfScales = 4;
	while (source.rows > 2 && source.cols > 2 && pyramidLevel < numberOfScales)
	{
		if(getHoleCount(inpaintMask[pyramidLevel]) == 0){
			pyramidLevel--;
			break;
		}
		pyramidLevel++;
		pyrDown(source, source);
		pyrDown(target, target);

		Mat newOff;
		for(int i = 0; i < KNN; i++){
			pyDownOffset(newOff, OffsetKNN[i],inpaintMask[pyramidLevel - 1], inpaintMask[pyramidLevel]);
			OffsetKNN[i] = newOff.clone();
		}

		pyrDown(Distance[pyramidLevel - 1], Distance[pyramidLevel]);
		Distance[pyramidLevel].setTo(0);

		Source[pyramidLevel] = source.clone();

	}

	for(int i = 0; i < pyramidLevel - 1; i++){
		inpaintMask[i].setTo(0);
	}
	
	Mat currentMask;
	Mat currentDistance;
	
	//image pyramid 
	for(int level = pyramidLevel; level >= 1; level--){
		source = Source[level].clone();
		currentMask = inpaintMask[level].clone();
		currentDistance = Distance[level].clone();

		//Mat needUpdateNNF = currentMask.clone();
		//currentMask.setTo(0);

		if(level == pyramidLevel){
			for(int i = 0; i < KNN; i++)
				conputeImageDistance(source, target, currentMask, OffsetKNN[i], currentDistanceKNN[i]);
		}
		else{
			
			Mat newOff = Mat(source.rows, source.cols, OFFSET_MAT_TYPE);
			for(int i = 0; i < KNN; i++){
				expandOffsetCopy(OffsetKNN[i], newOff);
			
				pyUpOffsetCopy(source, target, currentMask, OffsetKNN[i], newOff, currentDistanceKNN[i]);
				OffsetKNN[i] = newOff.clone();
			}
		}

		Mat newTarget;
		int loopTimes = 2;//min(2 * level, 1);
		int passTimes = 2;//min(level, 4);
		for(int loop = 1; loop <= loopTimes; loop++){
			jumpFlooding(source, target, OffsetKNN, currentMask, currentDistanceKNN, KNN);
			//currentMask.setTo(0);
			
			Mat newSource;
			bool upscale = false;
			if(loop == loopTimes){
				newSource = Source[level - 1].clone();
				resize(target, newTarget, newSource.size());
				currentMask = inpaintMask[level - 1];
				upscale = true;
			}
			else{
				newSource = Source[level].clone();
				newTarget = target.clone();
				upscale = false;
			}			
			
			vote(newSource, newTarget, OffsetKNN[0], currentMask, upscale, currentDistanceKNN[0]);
			target = newTarget.clone();
		}
		target = newTarget.clone();
		showTempImage(target, SourceImageRGB.size());
		cvWaitKey(33);
	}
	
	imshow("image", target);
	target.copyTo(SourceImageRGB);
	cvWaitKey(33);
}

void MyPatchMatch::imageInpaintWithRestrictCL_JF(Mat& SourceImageRGB, Mat TargetImageRGB, char* holeInRestrict){
	Mat inpaintMask[20];
	Mat Target[20];
	Mat Source[20];
	Mat Distance[20];
	Mat RestrictRegion[20];
	Mat Offset = Mat(SourceImageRGB.size(), OFFSET_MAT_TYPE);
	Offset.setTo(0);
	Offset.at<OFFSET_DATA_TYPE>(0,0)[0] = 0;
	this->patch_radius = 2;
	int pyramidLevel = 0;
	
	Mat source = SourceImageRGB.clone();
	Source[0] = source.clone();

	inpaintMask[0] = Mat(SourceImageRGB.size(), CV_8UC3);
	inpaintMask[0].setTo(0);

	Distance[0] = Mat(SourceImageRGB.size(), CV_16SC1);
	Distance[0].setTo(0);
	//Distance[0].at<unsigned short>(0,0) = 65536;

	RestrictRegion[0] = Mat(SourceImageRGB.size(), CV_8UC3);
	RestrictRegion[0].setTo(0);


	for(int row = 0; row < SourceImageRGB.rows; row++){
		for(int col = 0; col < SourceImageRGB.cols; col++){
			//只考虑在restrict区域内部的hole
			if(holeInRestrict[row * SourceImageRGB.cols + col] > 0)
				inpaintMask[0].at<cv::Vec3b>(row, col).val[0] = 255;
			else 
				inpaintMask[0].at<cv::Vec3b>(row, col).val[0] = 0;
			inpaintMask[0].at<cv::Vec3b>(row, col).val[1] = 0;
			inpaintMask[0].at<cv::Vec3b>(row, col).val[2] = 0;

			//初始化RestrictRegion
			if(InpaintRestrictMask[row * SourceImageRGB.cols + col] > 0)
				RestrictRegion[0].at<cv::Vec3b>(row, col).val[0] = 255;
			else 
				RestrictRegion[0].at<cv::Vec3b>(row, col).val[0] = 0;
			RestrictRegion[0].at<cv::Vec3b>(row, col).val[1] = 0;
			RestrictRegion[0].at<cv::Vec3b>(row, col).val[2] = 0;
		}
	}

	while (source.rows > 2 && source.cols > 2 && pyramidLevel < 20)
	{
		if(getHoleCount(inpaintMask[pyramidLevel]) == 0 || getHoleCount(RestrictRegion[pyramidLevel]) == 0)
			break;
		pyramidLevel++;
		Mat tempSource = source.clone();
		pyDownSource(source, inpaintMask[pyramidLevel-1], inpaintMask[pyramidLevel]);
		pyDownSource(tempSource, RestrictRegion[pyramidLevel-1], RestrictRegion[pyramidLevel]);

		Distance[pyramidLevel] = Mat(Distance[pyramidLevel-1].rows / 2, Distance[pyramidLevel-1].cols / 2, CV_16SC1);
		Distance[pyramidLevel].setTo(0);

		Source[pyramidLevel] = source.clone();
		//imshow("show1", source);
		//cvWaitKey();
	}



	Mat target = Source[pyramidLevel].clone();
	
	Mat currentMask;
	Mat currentDistance;
	
	//image pyramid 
	//kNN
	int KNN = 5;
	Mat OffsetKNN[16];
	Mat currentDistanceKNN[16];
	for(int level = pyramidLevel; level >= 1; level--){
		source = Source[level].clone();
		currentMask = inpaintMask[level].clone();
		currentDistance = Distance[level].clone();
		currentRestrict = RestrictRegion[level].clone();
		getRestrictPos();

		if(level == pyramidLevel){
			//initialOffset(source, target, currentMask, Offset, currentDistance);
			for(int i = 0; i < KNN; i++){
				//currentDistanceKNN[i] = Distance[level].clone();
				initialOffset(source, target, currentMask, OffsetKNN[i], currentDistanceKNN[i]);
			}
		}
		else{
			Mat newOff = Mat(currentMask.size(), OFFSET_MAT_TYPE);
			for(int i = 0; i < KNN; i++){			
				pyUpOffset(source, target, currentMask, OffsetKNN[i], newOff, currentDistanceKNN[i]);
				OffsetKNN[i] = newOff.clone();
			}
		}

		
		Mat newTarget;
		int loopTimes = max(2 * level, 3);
		int passTimes = max(level, 3);

		for(int loop = 1; loop <= loopTimes; loop++){
			jumpFloodingRestrict(source, target, OffsetKNN, currentMask, currentDistanceKNN, currentRestrict, KNN);
			Mat newSource;
			bool upscale = false;
			if(level >= 1 && loop == loopTimes){
				newSource = Source[level - 1];
				resize(target, newTarget, newSource.size());
				currentMask = inpaintMask[level - 1];
				upscale = true;
			}
			else{
				newSource = Source[level];
				newTarget = target.clone();
				upscale = false;
			}

			vote(newSource, newTarget, OffsetKNN[0], currentMask, upscale, currentDistanceKNN[0]);
			target = newTarget.clone();
		}
		target = newTarget.clone();
		showTempImage(target, SourceImageRGB.size());
		cvWaitKey(33);
	}
	
	imshow("image", target);
	target.copyTo(TargetImageRGB);
	//cvWaitKey(33);
}

void MyPatchMatch::jumpFlooding(Mat source, Mat target, Mat offset[], Mat mask, Mat currentDistance[], int KNN){
	int DATA_SIZE = source.rows * source.cols * 3;
	std::vector<char> S(DATA_SIZE), T(DATA_SIZE);
	std::vector<ushort> Off(DATA_SIZE * KNN), Dis(source.rows * source.cols * KNN);
	std::vector<uchar> Mask(DATA_SIZE / 3);
	int k = 0;
	ushort* offpointer;
	ushort* dispointer;
	for(int i = 0; i < KNN; i++){
		dispointer = &Dis[0] + i * source.rows * source.cols;
		for(int row = 0; row < source.rows; row++){
			for(int col = 0; col < source.cols; col++){
				dispointer[row * source.cols + col] = currentDistance[i].at<ushort>(row, col);
				for(int channel = 0; channel < 3; channel++){
					if(channel < 2){
						Off[k] = offset[i].at<OFFSET_DATA_TYPE>(row, col)[channel];
					}
					else{
						if(mask.at<Vec3b>(row, col)[0] > 0)
							int a = 1;
						Off[k] = mask.at<Vec3b>(row, col)[0];
					}
					k++;
				}
			}
		}
	}
	k = 0;
	for(int row = 0; row < source.rows; row++){
		for(int col = 0; col < source.cols; col++){
			for(int channel = 0; channel < 3; channel++){
				S[k] = source.at<Vec3b>(row, col)[channel];
				T[k] = target.at<Vec3b>(row, col)[channel];
				k++;
			}
			Mask[row * source.cols + col] = mask.at<Vec3b>(row, col)[0];
		}
	}


	int window_radius = max(source.rows, source.cols);

	
	cl_mem cl_source = clCreateBuffer(contextCl, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * DATA_SIZE, &S[0], NULL);
	cl_mem cl_target = clCreateBuffer(contextCl, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * DATA_SIZE, &T[0], NULL);
	cl_mem cl_offset = clCreateBuffer(contextCl, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_ushort) * DATA_SIZE * KNN, &Off[0], NULL);
	cl_mem cl_distance = clCreateBuffer(contextCl, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_ushort) * DATA_SIZE * KNN / 3, &Dis[0], NULL);
	cl_mem cl_mask = clCreateBuffer(contextCl, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_uchar) * DATA_SIZE / 3, &Mask[0], NULL);

	cl_uint4 imageSize = {source.rows, source.cols, window_radius, KNN};


	//set parameters
	clSetKernelArg(jumpFloodingKernel, 0, sizeof(cl_mem), &cl_source);
	clSetKernelArg(jumpFloodingKernel, 1, sizeof(cl_mem), &cl_target);
	clSetKernelArg(jumpFloodingKernel, 2, sizeof(cl_mem), &cl_offset);
	clSetKernelArg(jumpFloodingKernel, 3, sizeof(cl_mem), &cl_distance);
	clSetKernelArg(jumpFloodingKernel, 4, sizeof(cl_uint4), &imageSize);
	clSetKernelArg(jumpFloodingKernel, 5, sizeof(cl_mem), &cl_mask);
	
	size_t worksize[] = {source.rows, source.cols};
	cl_int err;
	err = clEnqueueNDRangeKernel(clQueue, jumpFloodingKernel, 2, 0, worksize, 0, 0, 0, 0);

	err = clEnqueueReadBuffer(clQueue, cl_distance, CL_TRUE, 0, sizeof(ushort) * DATA_SIZE / 3, &Dis[0], 0, 0, 0);
	err = clEnqueueReadBuffer(clQueue, cl_offset, CL_TRUE, 0, sizeof(ushort) * DATA_SIZE, &Off[0], 0, 0, 0);


	clReleaseMemObject(cl_source);
	clReleaseMemObject(cl_target);
	clReleaseMemObject(cl_distance);
	clReleaseMemObject(cl_offset);
	clReleaseMemObject(cl_mask);

		

	//copy to Mat
	k = 0;
	for(int i = 0; i < KNN; i++){
		//offpointer = &Off[0] + i * source.rows * source.cols * 3;
		dispointer = &Dis[0] + i * source.rows * source.cols;
		for(int row = 0; row < source.rows; row++){
			for(int col = 0; col < source.cols; col++){
				currentDistance[i].at<ushort>(row, col) = dispointer[row * source.cols + col];
				for(int channel = 0; channel < 3; channel++){					
					if(channel < 2){
						offset[i].at<OFFSET_DATA_TYPE>(row, col)[channel] = Off[k];
					}
					k++;
				}
			}
		}
	}
}

void MyPatchMatch::jumpFloodingCopy(Mat source, Mat target, Mat offset[], Mat mask, Mat currentDistance[], int KNN){
	int DATA_SIZE = source.rows * source.cols * 3;
	std::vector<char> S(DATA_SIZE), T(DATA_SIZE);
	std::vector<ushort> Off(DATA_SIZE * KNN), Dis(source.rows * source.cols * KNN);
	std::vector<uchar> Mask(DATA_SIZE / 3);
	int k = 0;
	ushort* offpointer;
	ushort* dispointer;
	for(int i = 0; i < KNN; i++){
		dispointer = &Dis[0] + i * source.rows * source.cols;
		for(int row = 0; row < source.rows; row++){
			for(int col = 0; col < source.cols; col++){
				dispointer[row * source.cols + col] = currentDistance[i].at<ushort>(row, col);
				for(int channel = 0; channel < 3; channel++){
					if(channel < 2){
						Off[k] = offset[i].at<OFFSET_DATA_TYPE>(row, col)[channel];
					}
					else{
						if(mask.at<Vec3b>(row, col)[0] > 0)
							int a = 1;
						Off[k] = mask.at<Vec3b>(row, col)[0];
					}
					k++;
				}
			}
		}
	}
	k = 0;
	for(int row = 0; row < source.rows; row++){
		for(int col = 0; col < source.cols; col++){
			for(int channel = 0; channel < 3; channel++){
				S[k] = source.at<Vec3b>(row, col)[channel];
				T[k] = target.at<Vec3b>(row, col)[channel];
				k++;
			}
			Mask[row * source.cols + col] = mask.at<Vec3b>(row, col)[0];
		}
	}


	int window_radius = max(source.rows, source.cols);

	
	cl_mem cl_source = clCreateBuffer(contextCl, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * DATA_SIZE, &S[0], NULL);
	cl_mem cl_target = clCreateBuffer(contextCl, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * DATA_SIZE, &T[0], NULL);
	cl_mem cl_offset = clCreateBuffer(contextCl, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_ushort) * DATA_SIZE * KNN, &Off[0], NULL);
	cl_mem cl_distance = clCreateBuffer(contextCl, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_ushort) * DATA_SIZE * KNN / 3, &Dis[0], NULL);
	cl_mem cl_mask = clCreateBuffer(contextCl, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_uchar) * DATA_SIZE / 3, &Mask[0], NULL);

	cl_uint4 imageSize = {source.rows, source.cols, window_radius, KNN};


	//set parameters
	clSetKernelArg(jumpFloodingCopyKernel, 0, sizeof(cl_mem), &cl_source);
	clSetKernelArg(jumpFloodingCopyKernel, 1, sizeof(cl_mem), &cl_target);
	clSetKernelArg(jumpFloodingCopyKernel, 2, sizeof(cl_mem), &cl_offset);
	clSetKernelArg(jumpFloodingCopyKernel, 3, sizeof(cl_mem), &cl_distance);
	clSetKernelArg(jumpFloodingCopyKernel, 4, sizeof(cl_uint4), &imageSize);
	clSetKernelArg(jumpFloodingCopyKernel, 5, sizeof(cl_mem), &cl_mask);
	
	size_t worksize[] = {source.rows, source.cols};
	cl_int err;
	err = clEnqueueNDRangeKernel(clQueue, jumpFloodingCopyKernel, 2, 0, worksize, 0, 0, 0, 0);

	err = clEnqueueReadBuffer(clQueue, cl_distance, CL_TRUE, 0, sizeof(ushort) * DATA_SIZE / 3, &Dis[0], 0, 0, 0);
	err = clEnqueueReadBuffer(clQueue, cl_offset, CL_TRUE, 0, sizeof(ushort) * DATA_SIZE, &Off[0], 0, 0, 0);


	clReleaseMemObject(cl_source);
	clReleaseMemObject(cl_target);
	clReleaseMemObject(cl_distance);
	clReleaseMemObject(cl_offset);
	clReleaseMemObject(cl_mask);

		

	//copy to Mat
	k = 0;
	for(int i = 0; i < KNN; i++){
		//offpointer = &Off[0] + i * source.rows * source.cols * 3;
		dispointer = &Dis[0] + i * source.rows * source.cols;
		for(int row = 0; row < source.rows; row++){
			for(int col = 0; col < source.cols; col++){
				currentDistance[i].at<ushort>(row, col) = dispointer[row * source.cols + col];
				for(int channel = 0; channel < 3; channel++){					
					if(channel < 2){
						offset[i].at<OFFSET_DATA_TYPE>(row, col)[channel] = Off[k];
					}
					k++;
				}
			}
		}
	}
}

void MyPatchMatch::jumpFloodingRestrict(Mat source, Mat target, Mat offset[], Mat mask, Mat currentDistance[], Mat restrict, int KNN){
	int DATA_SIZE = source.rows * source.cols * 3;
	std::vector<char> S(DATA_SIZE), T(DATA_SIZE);
	std::vector<ushort> Off(DATA_SIZE * KNN), Dis(source.rows * source.cols * KNN);
	std::vector<uchar> Mask(DATA_SIZE / 3), Restrict(DATA_SIZE / 3);
	int k = 0;
	ushort* offpointer;
	ushort* dispointer;
	for(int i = 0; i < KNN; i++){
		dispointer = &Dis[0] + i * source.rows * source.cols;
		for(int row = 0; row < source.rows; row++){
			for(int col = 0; col < source.cols; col++){
				dispointer[row * source.cols + col] = currentDistance[i].at<ushort>(row, col);
				for(int channel = 0; channel < 3; channel++){
					if(channel < 2){
						Off[k] = offset[i].at<OFFSET_DATA_TYPE>(row, col)[channel];
					}
					else{
						if(mask.at<Vec3b>(row, col)[0] > 0)
							int a = 1;
						Off[k] = mask.at<Vec3b>(row, col)[0];
					}
					k++;
				}
			}
		}
	}
	k = 0;
	for(int row = 0; row < source.rows; row++){
		for(int col = 0; col < source.cols; col++){
			for(int channel = 0; channel < 3; channel++){
				S[k] = source.at<Vec3b>(row, col)[channel];
				T[k] = target.at<Vec3b>(row, col)[channel];
				k++;
			}
			Mask[row * source.cols + col] = mask.at<Vec3b>(row, col)[0];
			Restrict[row * source.cols + col] = restrict.at<Vec3b>(row, col)[0];
		}
	}


	int window_radius = max(source.rows, source.cols);

	
	cl_mem cl_source = clCreateBuffer(contextCl, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * DATA_SIZE, &S[0], NULL);
	cl_mem cl_target = clCreateBuffer(contextCl, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * DATA_SIZE, &T[0], NULL);
	cl_mem cl_offset = clCreateBuffer(contextCl, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_ushort) * DATA_SIZE * KNN, &Off[0], NULL);
	cl_mem cl_distance = clCreateBuffer(contextCl, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_ushort) * DATA_SIZE * KNN / 3, &Dis[0], NULL);
	cl_mem cl_mask = clCreateBuffer(contextCl, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_uchar) * DATA_SIZE / 3, &Mask[0], NULL);
	cl_mem cl_restrict = clCreateBuffer(contextCl, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_uchar) * DATA_SIZE / 3, &Restrict[0], NULL);
	cl_uint4 imageSize = {source.rows, source.cols, window_radius, KNN};


	//set parameters
	clSetKernelArg(jumpFloodingRestrictKernel, 0, sizeof(cl_mem), &cl_source);
	clSetKernelArg(jumpFloodingRestrictKernel, 1, sizeof(cl_mem), &cl_target);
	clSetKernelArg(jumpFloodingRestrictKernel, 2, sizeof(cl_mem), &cl_offset);
	clSetKernelArg(jumpFloodingRestrictKernel, 3, sizeof(cl_mem), &cl_distance);
	clSetKernelArg(jumpFloodingRestrictKernel, 4, sizeof(cl_uint4), &imageSize);
	clSetKernelArg(jumpFloodingRestrictKernel, 5, sizeof(cl_mem), &cl_mask);
	clSetKernelArg(jumpFloodingRestrictKernel, 6, sizeof(cl_mem), &cl_restrict);
	
	size_t worksize[] = {source.rows, source.cols};
	cl_int err;
	err = clEnqueueNDRangeKernel(clQueue, jumpFloodingRestrictKernel, 2, 0, worksize, 0, 0, 0, 0);

	err = clEnqueueReadBuffer(clQueue, cl_distance, CL_TRUE, 0, sizeof(ushort) * DATA_SIZE / 3, &Dis[0], 0, 0, 0);
	err = clEnqueueReadBuffer(clQueue, cl_offset, CL_TRUE, 0, sizeof(ushort) * DATA_SIZE, &Off[0], 0, 0, 0);


	clReleaseMemObject(cl_source);
	clReleaseMemObject(cl_target);
	clReleaseMemObject(cl_distance);
	clReleaseMemObject(cl_offset);
	clReleaseMemObject(cl_mask);
	clReleaseMemObject(cl_restrict);

		

	//copy to Mat
	k = 0;
	for(int i = 0; i < KNN; i++){
		//offpointer = &Off[0] + i * source.rows * source.cols * 3;
		dispointer = &Dis[0] + i * source.rows * source.cols;
		for(int row = 0; row < source.rows; row++){
			for(int col = 0; col < source.cols; col++){
				currentDistance[i].at<ushort>(row, col) = dispointer[row * source.cols + col];
				for(int channel = 0; channel < 3; channel++){					
					if(channel < 2){
						offset[i].at<OFFSET_DATA_TYPE>(row, col)[channel] = Off[k];
					}
					k++;
				}
			}
		}
	}
}

void MyPatchMatch::jumpFloodingCPU(Mat source, Mat target, Mat offset[], Mat mask, Mat currentDistance[], int KNN){
	int DATA_SIZE = source.rows * source.cols * 3;
	std::vector<char> S(DATA_SIZE), T(DATA_SIZE);
	std::vector<ushort> Off(DATA_SIZE * KNN), Dis(source.rows * source.cols * KNN);
	std::vector<uchar> Mask(DATA_SIZE / 3);

	std::vector<ushort> NewOff(DATA_SIZE * KNN), NewDis(source.rows * source.cols * KNN);
	int k = 0;
	ushort* offpointer;
	ushort* dispointer;
	for(int i = 0; i < KNN; i++){
		//offpointer = &Off[0] + i * source.rows * source.cols * 3;
		dispointer = &Dis[0] + i * source.rows * source.cols;

		for(int row = 0; row < source.rows; row++){
			for(int col = 0; col < source.cols; col++){
				dispointer[row * source.cols + col] = currentDistance[i].at<ushort>(row, col);
				for(int channel = 0; channel < 3; channel++){
					if(channel < 2){
						Off[k] = offset[i].at<OFFSET_DATA_TYPE>(row, col)[channel];
					}
					else{
						if(mask.at<Vec3b>(row, col)[0] > 0)
							int a = 1;
						Off[k] = mask.at<Vec3b>(row, col)[0];
					}
					k++;
				}
			}
		}
	}

	k = 0;
	for(int row = 0; row < source.rows; row++){
		for(int col = 0; col < source.cols; col++){
			for(int channel = 0; channel < 3; channel++){
				S[k] = source.at<Vec3b>(row, col)[channel];
				T[k] = target.at<Vec3b>(row, col)[channel];
				k++;
			}
			Mask[row * source.cols + col] = mask.at<Vec3b>(row, col)[0];
		}
	}

	int window_radius = max(source.rows, source.cols);

	int knn = KNN;
	//cl_uint4 imageSize = {source.rows, source.cols, window_radius, KNN};
	int NNFOFFSET = DATA_SIZE;
	int DISOFFSET = DATA_SIZE / 3;
	for(int hhh= 0; hhh < 2; hhh++)
	{
		window_radius *= 4;
		while(window_radius > 1){
			window_radius /= 2;
		
			for(int row = 0; row < source.rows; row++){
				for(int col = 0; col < source.cols; col++){
					int ROW[8];
					int COL[8];
	
					int rows[9 * 17];
					int cols[9 * 17];
					int dis[9 * 17];

					ROW[0] = row - window_radius;
					COL[0] = col - window_radius;
	
					ROW[1] = row - window_radius;
					COL[1] = col;
	
					ROW[2] = row - window_radius;
					COL[2] = col + window_radius;
	
					ROW[3] = row;
					COL[3] = col - window_radius;
	
					ROW[4] = row;
					COL[4] = col + window_radius;
	
					ROW[5] = row + window_radius;
					COL[5] = col - window_radius;
	
					ROW[6] = row + window_radius;
					COL[6] = col;
	
					ROW[7] = row + window_radius;
					COL[7] = col + window_radius;
		
					int k = 0;
					if(row > 2 && col > 2)
						int a = 1;
					for(int i = 0; i < knn; i++){
						dis[k] = Dis[i * DISOFFSET + row * source.cols + col];
						rows[k] = Off[i * NNFOFFSET + row * source.cols * 3 + col * 3 + 1];
						cols[k] = Off[i * NNFOFFSET + row * source.cols * 3 + col * 3];
						k++;
					}
					for(int i = 0; i < 8; i++){
						if(ROW[i] < 0 || COL[i] < 0 || ROW[i] >= source.rows || COL[i] >= source.cols)
							continue;
						//if(ROW[i] < 0) ROW[i] = 0;
						//if(ROW[i] >= imageSize.x) ROW[i] = imageSize.x - 1;
						//if(COL[i] < 0) COL[i] = 0;
						//if(COL[i] >= imageSize.y) COL[i] = imageSize.y - 1;
		
						//dis[k] = computeDistance(target, row, col, source, ROW[i], COL[i], imageSize, mask);
						//rows[k] = ROW[i];
						//cols[k] = COL[i];
						//k++;
		
						for(int j = 0; j < knn; j++){	
							int currentRow = Off[j * NNFOFFSET + ROW[i] * source.cols * 3 + COL[i] * 3 + 1];
							int currentCol = Off[j * NNFOFFSET + ROW[i] * source.cols * 3 + COL[i] * 3];
			
							dis[k] = computeDistance(target, row, col, source, currentRow, currentCol, mask);
							rows[k] = currentRow;
							cols[k] = currentCol;
							k++;
						}
					}

					for(int i = 0; i < knn; i++){
						for(int j = i + 1; j < k; j++){
							if(rows[i] == rows[j] && cols[i] == cols[j]){
								ushort c = dis[j];
								dis[j] = dis[k-1];
								dis[k-1] = c;
				
								c = rows[k-1];
								rows[k-1] = rows[j];
								rows[j] = c;
				
								c = cols[k-1];
								cols[k-1] = cols[j];
								cols[j] = c;
								k--;
								j--;
								continue;
							}
							if(dis[i] > dis[j]){
								ushort c = dis[j];
								dis[j] = dis[i];
								dis[i] = c;
				
								c = rows[i];
								rows[i] = rows[j];
								rows[j] = c;
				
								c = cols[i];
								cols[i] = cols[j];
								cols[j] = c;
							}
			
						}
					}

					for(int i = 0; i < knn; i++){
						NewDis[i * DISOFFSET + row * source.cols + col] = dis[i];
						NewOff[i * NNFOFFSET + row * source.cols * 3 + col * 3] = cols[i];
						NewOff[i * NNFOFFSET + row * source.cols * 3 + col * 3 + 1] = rows[i];
						NewOff[i * NNFOFFSET + row * source.cols * 3 + col * 3 + 2] = 0;
					}
				}
			}
		

			for(int row = 0; row < source.rows; row++){
				for(int col = 0; col < source.cols; col++){
					for(int i = 0; i < knn; i++){
						Dis[i * DISOFFSET + row * source.cols + col] = NewDis[i * DISOFFSET + row * source.cols + col];
						Off[i * NNFOFFSET + row * source.cols * 3 + col * 3] = NewOff[i * NNFOFFSET + row * source.cols * 3 + col * 3];
						Off[i * NNFOFFSET + row * source.cols * 3 + col * 3 + 1] = NewOff[i * NNFOFFSET + row * source.cols * 3 + col * 3 + 1];
						Off[i * NNFOFFSET + row * source.cols * 3 + col * 3 + 2] = NewOff[i * NNFOFFSET + row * source.cols * 3 + col * 3 + 2];
					}
				}
			}

			//copy to Mat
			k = 0;
			for(int i = 0; i < KNN; i++){
				//offpointer = &Off[0] + i * source.rows * source.cols * 3;
				dispointer = &Dis[0] + i * source.rows * source.cols;
				for(int row = 0; row < source.rows; row++){
					for(int col = 0; col < source.cols; col++){
						currentDistance[i].at<ushort>(row, col) = dispointer[row * source.cols + col];
						for(int channel = 0; channel < 3; channel++){					
							if(channel < 2){
								offset[i].at<OFFSET_DATA_TYPE>(row, col)[channel] = Off[k];
							}
							k++;
						}
					}
				}
			}
			debugShowOffset("asdasasdasdadsdfsdfsdfsdads", offset[0]);
			cvWaitKey(33);
		}
	}
	
}

