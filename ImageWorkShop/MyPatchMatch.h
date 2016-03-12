#ifndef MY_PATCH_MATCH_H
#define MY_PATCH_MATCH_H
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>

#include <CL\cl.h>
#include<CL\opencl.h>
#define PATCHRADIUS 2
#define PATCHSIZE 5

#include<vector>



using namespace cv;
enum INTERECTYION{NO, /*HOLE, TARGET, SOURCE,*/ ImageCopy, INPAINT};
enum ImageCopyMODE{ADDTARGET, MOVE, NO_ImageCopy};
enum ImageInpaintMODE{INPAINT_NO, INPAINT_HOLE, INPAINT_RESTRICT};
enum PatchMatchMode{GPU_JUMPFLOODING, CPU};
class MyPatchMatch{
public:
	int numberOfScales;
	int numberOfIterationPerScale;
	int imageRows;
	int imageCols;
	Mat tempImage;
	int mouseDownX;
	int mouseDownY;
	INTERECTYION interection;
	MyPatchMatch();
	~MyPatchMatch();
	

	// old inpaint method
	void imageInpaint3(Mat& SourceImageRGB, Mat TargetImageRGB);
	void initHole(cv::Mat image, cv::Mat offset);
	void pyrDownMask(cv::Mat in, cv::Mat &out);
	void pyrUpOffset(Mat in, Mat mask, Mat &out);
	void generateHole(Mat& target, Mat source, Mat offset, bool odd);
	Vec3f generateHolePixel(Mat& target, Mat source, Mat offset, int pixel_row, int pixel_col, bool odd);
	Vec3f getMeanHolePixel(float* weight, Vec3f* pixels, int length, bool odd);
	void randomSearchHole(Mat source, Mat target, Mat& offest, int row, int col);
	void propagationHole(Mat source, Mat target, Mat& offest, int row, int col, bool odd);
	void randomSearch(Mat source, Mat target, Mat& offest);
	void propagation(Mat source, Mat target, Mat& offest, bool odd);
	void vote2(Mat source, Mat offset, Mat &result, Mat mask);
	float computeDistance(Mat A, Mat B);
	float computeDistanceWithHole(Mat A, Mat B, Mat offset, int low_row, int left_col);
	Mat getPatch(int topLeftX, int topLeftY, Mat source);
	void scale(Mat& image, int scaleTime);
	void initialOffset(Mat &offset, bool ifReserve);


	// old image copy
	double w;
	int oldTopLeftX, oldTopLeftY;
	int newTopLeftX, newTopLeftY;
	int ImageCopyRadius;
	char *ImageCopyMask;
	ImageCopyMODE ImageCopyMode;	
	bool selectImageCopyPatch;
	bool ImageCopyMoveStart;

	void vote(Mat source, Mat offset, Mat &result);
	void getAndShowImageCopyMovedImage(int ax, int ay, Mat& image, bool ifImageCopy);
	void addToImageCopyMask(int ax, int ay, int rows, int cols);
	void zeroImageCopyMask(int length, bool newImage);
	Rect getImageCopyMaskRect();
	void showImageCopyMaskImage(Mat &image);
	void imageImageCopy(Mat& SourceImageRGB, Mat TargetImageRGB);
	void imageImageCopy2(Mat& SourceImageRGB, Mat TargetImageRGB);
	void updateOffset(Mat& offsetMap, Mat newOffsetMap, Mat target, Mat source);
	void expandOffset(Mat oldOffset, Mat& newOffset);
	Mat displayRegartImage(Mat& Image, Size orgSize);
	void addCircleToImage(cv::Mat &image, int ax, int ay);
	


	//new inpaint method
	Mat currentRestrict;
	double *similarity;
	int ImageInpaintRadius;
	int ImageInpaintRestrictRadius;
	int patch_radius;
	int restrictUp, restrictDown, restrictLeft, restrictRight;
	int erodeRadius;
	ImageInpaintMODE ImageInpaintMode;
	ImageInpaintMODE ImageInpaintModeWhenWork;


	
	void imageInpaint(Mat& SourceImageRGB, Mat TargetImageRGB);
	void imageInpaintWithRestrict(Mat& SourceImageRGB, Mat TargetImageRGB, char* holeInRestrict);	
	void pyDownSource(Mat& source, Mat& mask, Mat& newMask);
	void pyUpTarget(Mat& oldTarget, Mat& newTarget);
	int getHoleCount(Mat& source);
	void initialOffset(Mat source, Mat target, Mat mask, Mat &Offset, Mat& dis);
	void updatePixelNNF(Mat source, Mat target, Mat offset, Mat mask, int row, int col, bool odd, Mat& currentDistance);
	int computeDistance(Mat source, int srow, int scol, Mat target, int trow, int tcol, Mat mask);
	void vote(Mat source, Mat target, Mat& offset, Mat mask, bool upscale, Mat& distance);
	void pyUpOffset(Mat source, Mat target, Mat mask, Mat& oldOff, Mat& newOff, Mat &currentDistance);	
	void initialSimilarity();
	void deleteSimilarity();
	void debugoffsettxt(Mat Offset, Mat dis);
	void showImageInpaintRestrictMaskImage(Mat &image);
	char *InpaintRestrictMask;
	void erodeMask(int rows, int cols);
	void imageInpaint2(Mat& SourceImageRGB, Mat TargetImageRGB);
	void showTempImage(Mat image, Size originSize);
	char *ImageInpaintMask;
	void getRestrictPos();
	void addToImageInpaintMask(int ax, int ay, int rows, int cols);
	void addToImageInpaintRestrictMask(int ax, int ay, int rows, int cols);
	void zeroInpaintMask(int length, bool newImage);
	void initialImageInpaintOffset(Mat &offset, Mat mask);
	void setOffsetMask(Mat &offset, Mat mask);
	void showImageInpaintMaskImage(Mat &image);
	void addCircleToImageInpaint(cv::Mat &image, int ax, int ay);


	//new copy method
	void imageInpaintCopy(Mat& SourceImageRGB, Mat TargetImageRGB);
	void initialCopyOffset(Mat &offset, Mat &mask);
	void conputeImageDistance(Mat source, Mat target, Mat mask, Mat &Offset, Mat& dis);
	void pyUpOffsetCopy(Mat source, Mat target, Mat mask, Mat& oldOff, Mat& newOff, Mat &currentDistance);	
	void pyDownOffset(Mat& newoff, Mat& oldoff);
	void pyDownOffset(Mat& newOff, Mat& oldOff, Mat& oldmask, Mat& newmask);
	void pyDownSourceCopy(Mat& source, Mat& mask, Mat& newMask);
	void expandOffsetCopy(Mat oldOffset, Mat& newOffset);


	//interact
	void dealWithImageInpaintMouseMoveEvent(int ax, int ay, Mat &image);
	void dealWithImageInpaintMouseButtonDown(int ax, int ay, Mat &image);
	void dealWithImageInpaintMouseButtonUp(int ax, int ay, Mat &image);

	void dealWithImageCopyMouseMoveEvent(int ax, int ay, Mat &image);
	void dealWithImageCopyMouseMoveButtonDown(int ax, int ay, Mat &image);
	bool dealWithImageCopyMouseMoveButtonUp(int ax, int ay, Mat &image);

	
	//for debug
	bool debugOffset(Mat& offset);
	bool debugShowOffset(string name, Mat offset);
	void debugImage(Mat &Image, char * name);
	void debugMat(Mat mat);


	//opencl
	PatchMatchMode mode;
	vector<cl_platform_id> platforms;
	vector<cl_device_id> device_id;
	cl_context contextCl;
	cl_command_queue clQueue;

	cl_program jumpFloodingProgram;
	cl_program jumpFloodingCopyProgram;
	cl_program jumpFloodingRestrictProgram;

	cl_kernel jumpFloodingKernel;
	cl_kernel jumpFloodingCopyKernel;
	cl_kernel jumpFloodingRestrictKernel;

	cl_int buildProgram(char* filename, char* kernel_name, cl_kernel& kernel, cl_program &program);
	void initOpencl();
	void destroyOpencl();

	void imageInpaintCL_JF(Mat& SourceImageRGB, Mat TargetImageRGB);
	void imageInpaintCL_JF_TEST(Mat& SourceImageRGB, Mat TargetImageRGB);

	void imageInpaintCopyCL_JF(Mat& SourceImageRGB, Mat TargetImageRGB);

	void imageInpaintWithRestrictCL_JF(Mat& SourceImageRGB, Mat TargetImageRGB, char* holeInRestrict);
	
	void jumpFloodingCopy(Mat source, Mat target, Mat offset[], Mat mask, Mat currentDistance[], int KNNnum);
	void jumpFlooding(Mat source, Mat target, Mat offset[], Mat mask, Mat currentDistance[], int KNNnum);
	void jumpFloodingRestrict(Mat source, Mat target, Mat offset[], Mat mask, Mat currentDistance[], Mat restrict, int KNNnum);


	void jumpFloodingCPU(Mat source, Mat target, Mat offset[], Mat mask, Mat currentDistance[], int KNNnum);
};

#endif