#ifndef IMAGEWORKSHOP_H
#define IMAGEWORKSHOP_H

#include <QtWidgets/QMainWindow>
#include "ui_imageworkshop.h"
#include <qimage.h>
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>
#include "Vignette.h"
#include "GaussFilter.h"
#include "MedianFilter.h"
#include "ImageSharpening.h"
//#include "PatchMatch.h"
#include "WhiteBalance.h"
#include "Saturation.h"
#include "Liquify.h"
#include "Lomo.h"
#include "FaceBeautification.h"
#include "ColorTransfer.h"
#include "MyPatchMatch.h"
//#include "ImageWidget.h"
enum LomoMode{NOMODE, SEPIA, PENCIL, CARVING, LOMO1};
enum Active{NONE, LIQUIFY, PATCHMATCH, LOMO, SKINMASK};
class ImageWorkShop : public QMainWindow
{
	Q_OBJECT

public:
	ImageWorkShop(QWidget *parent = 0);
	~ImageWorkShop();
	//void initMenus();
	QImage *qImage;
	QImage *qOriginImage;
	cv::Mat cvImage;
	cv::Mat cvOriginImage;
	Vignette vignette;
	MedianFilter medianFilter;
	GaussFilter gaussFilter;
	ImageSharp imageSharp;
	//PatchMatch patchMatch;
	MyPatchMatch myPatchMatch;
	WhiteBalance whiteBalance;
	Saturation saturation;
	Liquify liquify;
	Lomo lomo;
	FaceBeautification faceBeautification;
	//ColorTransfer colorTransfer;

	//BITMAP *ann;
	cv::Mat target;
	int brushSize;
	int skinMaskRadius;
	int whitenThreshold;
	cv::Mat skinMask;
	Active active;
	LomoMode lomoMode;
	void addToSkinMask(int ax, int ay);
	void addCircle(cv::Mat &img, int ax, int ay);
	void addMaskToImage(cv::Mat &img);

	cv::Mat colorMapSource;
	cv::Mat colorMapTarget;
private:
	//ImageWidget *imagewidget;
	bool loadImage;
	bool loadtargetimage;
	std::string name;
	Ui::ImageWorkShopClass ui;
	void initImageWorkShop();
	void zeroImage(cv::Mat &img);
	QImage* MatToQimage(cv::Mat &cvImage);

friend void onMouse(int event,int x,int y,int flags,void* param);
public slots:
	void slotOpenFile();
	void slotSaveFile();
	void slotStageSave();
	void slotStageBack();
	void slotVignetteFactor();
	void slotMedian();
	void slotGauss();
	void slotCompare();
	void slotSharp();
	void slotWhiteBalance();

	//void slotTargetImage();
	//void slotBrushRadius();
	void slotSaturation();

	void slotLiquifySetRadius();
	void slotLiquify();
	void slotLomo();
	void slotLomoR();
	void slotLomoG();
	void slotLomoB();
	void slotExfoliating();
	void slotWhitenSkin();
	//void slotDrawSkinMask();
	void slotDrawSkinMaskRadius();
	void slotWhitenThreshold();

	void slotColorMapSource();
	void slotColorMapTarget();
	void slotColorMapResult();
	void slotColorMapGrayToRGB();

	void slotImageCopyRadius();
	void slotImageCopyMovePatch();

	void slotImageInpaintRadius();
	void slotImageInpaintRestrictRadius();
	void slotImageInpaintResult();
	void slotImageInpaintRestrictResult();
	void slotImageInpaintRestrictResultOut();
	void slotRrodeRadius();

	void slotPatchMatchIfGpu();

	void slotClearRestrict();
	void slotClearRegion();

};


#endif // IMAGEWORKSHOP_H
