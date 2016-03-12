#include "imageworkshop.h"
#include <QLabel>
#include <qfiledialog.h>
#include <qmessagebox.h>

ImageWorkShop::ImageWorkShop(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	
	brushSize = 1;
	skinMaskRadius = 5;
	whitenThreshold = 0;
	active = NONE;
	lomoMode = NOMODE;
	QString lomoStr[] = {"", "SEPIA", "PENCIL", "CARVING", "LOMO1"};

	for(int i = 0; i < 5; i++)
		ui.comboBox_Lomo->addItem(lomoStr[i]);

	QString patchmatchifgpu[] = {"GPU_JF", "CPU"};
	for(int i = 0; i < 2; i++)
		ui.comboBox_ifGpu->addItem(patchmatchifgpu[i]);
	initImageWorkShop();

	////opencl JumpFlooding test
	//QString filename;
	//filename = "C:\\Users\\qinghua\\Documents\\Visual Studio 2012\\Projects\\ImageWorkShop\\ImageWorkShop\\22222.jpg";
	//std::string str = filename.toLocal8Bit();
	//cvImage = cv::imread(str) ;
	//Mat temp;
	//cvImage.copyTo(temp);
	//filename = "C:\\Users\\qinghua\\Documents\\Visual Studio 2012\\Projects\\ImageWorkShop\\ImageWorkShop\\11111.jpg";
	//str = filename.toLocal8Bit();
	//cvImage = cv::imread(str) ;
	//myPatchMatch.imageInpaintCL_JF_TEST(temp, cvImage);
}

ImageWorkShop::~ImageWorkShop()
{
	//if(qImage != NULL)
	//	delete qImage;
	//if(qOriginImage != NULL)
	//	delete qOriginImage;
}

void ImageWorkShop::initImageWorkShop(){
	//imagewidget = new ImageWidget(ui.centralWidget);
	loadImage = false;
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(slotOpenFile()));
	connect(ui.actionSave_as, SIGNAL(triggered()), this, SLOT(slotSaveFile()));
	connect(ui.actionImage_last_saved, SIGNAL(triggered()), this, SLOT(slotCompare()));
	connect(ui.actionStageSave, SIGNAL(triggered()), this, SLOT(slotStageSave()));
	connect(ui.actionStageBack, SIGNAL(triggered()), this, SLOT(slotStageBack()));
	connect(ui.horizontalSliderVignette, SIGNAL(actionTriggered(int)), this, SLOT(slotVignetteFactor()));
	connect(ui.pushButtonMedian, SIGNAL(clicked()), this, SLOT(slotMedian()));
	connect(ui.horizontalSliderGauss, SIGNAL(actionTriggered(int)), this, SLOT(slotGauss()));
	connect(ui.horizontalSliderBalance, SIGNAL(actionTriggered(int)), this, SLOT(slotWhiteBalance()));
	connect(ui.horizontalSliderSaturation, SIGNAL(actionTriggered(int)), this, SLOT(slotSaturation()));

	connect(ui.pushButtonSharp, SIGNAL(clicked()), this, SLOT(slotSharp()));

	connect(ui.pushButtonLiquify, SIGNAL(clicked()), this, SLOT(slotLiquify()));
	connect(ui.pushButtonLiquifyRadius, SIGNAL(clicked()), this, SLOT(slotLiquifySetRadius()));

	connect(ui.comboBox_Lomo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotLomo()));
	connect(ui.horizontalSliderLomoR, SIGNAL(actionTriggered(int)), this, SLOT(slotLomoR()));
	connect(ui.horizontalSliderLomoG, SIGNAL(actionTriggered(int)), this, SLOT(slotLomoG()));
	connect(ui.horizontalSliderLomoB, SIGNAL(actionTriggered(int)), this, SLOT(slotLomoB()));

	connect(ui.pushButton_Exfoliating, SIGNAL(clicked()), this, SLOT(slotExfoliating()));

	connect(ui.horizontalSliderWhitenSkin, SIGNAL(actionTriggered(int)), this, SLOT(slotWhitenSkin()));

	connect(ui.horizontalSliderWhitenSkinMask, SIGNAL(actionTriggered(int)), this, SLOT(slotDrawSkinMaskRadius()));
	connect(ui.horizontalSliderThreshold, SIGNAL(actionTriggered(int)), this, SLOT(slotWhitenThreshold()));

	connect(ui.pushButton_Source, SIGNAL(clicked()), this, SLOT(slotColorMapSource()));
	connect(ui.pushButton_Target, SIGNAL(clicked()), this, SLOT(slotColorMapTarget()));
	connect(ui.pushButton_Result, SIGNAL(clicked()), this, SLOT(slotColorMapResult()));
	connect(ui.pushButton_ResultGrayToRGB, SIGNAL(clicked()), this, SLOT(slotColorMapGrayToRGB()));


	connect(ui.horizontalSliderBrushRadiusImageCopy, SIGNAL(actionTriggered(int)), this, SLOT(slotImageCopyRadius()));
	connect(ui.pushButtonMovePatch, SIGNAL(clicked()), this, SLOT(slotImageCopyMovePatch()));

	connect(ui.horizontalSliderBrushRadiusImageInpaintHole, SIGNAL(actionTriggered(int)), this, SLOT(slotImageInpaintRadius()));
	connect(ui.pushButtonImageInpaintHoleResult, SIGNAL(clicked()), this, SLOT(slotImageInpaintResult()));

	connect(ui.horizontalSliderBrushRadiusImageInpaintHoleRestrict, SIGNAL(actionTriggered(int)), this, SLOT(slotImageInpaintRestrictRadius()));
	connect(ui.pushButtonImageInpaintHoleRestrictResult, SIGNAL(clicked()), this, SLOT(slotImageInpaintRestrictResult()));
	connect(ui.pushButtonImageInpaintHoleRestrictResultOut, SIGNAL(clicked()), this, SLOT(slotImageInpaintRestrictResultOut()));
	connect(ui.pushButtonErodeRadius, SIGNAL(clicked()), this, SLOT(slotRrodeRadius()));
	connect(ui.comboBox_ifGpu, SIGNAL(currentIndexChanged(int)), this, SLOT(slotPatchMatchIfGpu()));

	connect(ui.pushButton_ClearRestrict, SIGNAL(clicked()), this, SLOT(slotClearRestrict()));
	connect(ui.pushButton_ClearRegion, SIGNAL(clicked()), this, SLOT(slotClearRegion()));
	
}
//void ImageWorkShop::initMenus(){
//	
//}

void ImageWorkShop::slotOpenFile(){
	QString filename;
    filename= QFileDialog::getOpenFileName(this,
                                          tr("open file"), "",
                                          tr("Images (*.png *.bmp *.jpeg *.jpg)")); 
	//filename = "C:\\Users\\qinghua\\Documents\\Visual Studio 2012\\Projects\\ImageWorkShop\\ImageWorkShop\\6.png";
	if(filename.isEmpty())
    { 
		return;
    } 
	else {
		loadImage = true;
		
		loadtargetimage = false;
		std::string str = filename.toLocal8Bit();
		cvImage = cv::imread(str) ;
		cvImage.copyTo(cvOriginImage);
		//qImage = MatToQimage(cvImage);
		
		QFileInfo fi = QFileInfo(filename);
		name = fi.fileName().toLocal8Bit();
		name = "image";
		cv::imshow(name, cvImage) ;

		//faceBeautification.bilateralFilter(cvImage, 3);
		//cv::imshow("12", cvImage) ;
		cv::setMouseCallback("image", onMouse, 0);
		cvOriginImage.copyTo(skinMask);
		zeroImage(skinMask);
		myPatchMatch.zeroImageCopyMask(cvImage.rows * cvImage.cols, true);
		myPatchMatch.zeroInpaintMask(cvImage.rows * cvImage.cols, true);
		//patchMatch.zeroMask(cvImage.rows * cvImage.cols, true);
		//faceBeautification.genFaceMask(cvImage);
		//loadfile();
		//ui.labelImage->setPixmap(QPixmap::fromImage(*qImage));
		ui.comboBox_Lomo->setCurrentIndex(0);
    }
}

void ImageWorkShop::slotSaveFile(){
	QString fileName;
	QString slcStr;
	if(!loadImage){
		QMessageBox::information(this,
                                     tr("you did not load any image"),
                                     tr("image save fail"));
		return;
	}
    fileName= QFileDialog::getSaveFileName(this,
                                          tr("save file"), "",
                                          tr("*.bmp;; *.png;; *.jpeg"), &slcStr); 
	if(fileName.isEmpty())
    { 
		return;
    } 
	else {   
		if (QFileInfo(fileName).suffix().isEmpty()){
			if(slcStr.right(3)=="bmp")
			{
				fileName+=".bmp";
			}
			if(slcStr.right(3)=="png")
			{
				fileName+=".png";
			}
			if(slcStr.right(4)=="jpeg")
			{
				fileName+=".jpeg";
			}
		}
		std::string str = fileName.toLocal8Bit();
		cv::imwrite(str, cvImage) ;

    }
}
QImage* ImageWorkShop::MatToQimage(cv::Mat &cvImage){
	cv::Mat img;  
	QImage *result;    
	//switch(cvImage.depth()){
	//	case 
	//}
	cvtColor(cvImage, img, CV_BGR2RGB);  
	result = new QImage((unsigned char*)img.data, // uchar* data  
        img.cols, img.rows, // width height  
       // img.step, //bytesPerLine  
        QImage::Format_RGB888);
	return result;
}

void ImageWorkShop::slotVignetteFactor(){
	if(!loadImage){
		return;
	}
	ui.comboBox_Lomo->setCurrentIndex(0);
	active = NONE;
	double factor = (ui.horizontalSliderVignette->value() + 6) * 1.0 / 10;
	cvOriginImage.copyTo(cvImage);
	vignette.vignette(cvImage, factor);

	QString s = QString::number(factor,'f',3);
	ui.lineEditVignette->setText(s);
	cv::imshow("image", cvImage);
}
void ImageWorkShop::slotMedian(){
	if(!loadImage){
		return;
	}
	active = NONE;
	ui.comboBox_Lomo->setCurrentIndex(0);
	int radius = ui.spinBoxMedian->value();
	cvOriginImage.copyTo(cvImage);
	medianFilter.medianFilter(cvImage, radius);
	cv::imshow("image", cvImage);
}
void ImageWorkShop::slotStageSave(){
	if(!loadImage){
		return;
	}
	active = NONE;
	cvImage.copyTo(cvOriginImage);
	ui.comboBox_Lomo->setCurrentIndex(0);
}
void ImageWorkShop::slotStageBack(){
	if(!loadImage){
		return;
	}
	active = NONE;
	ui.comboBox_Lomo->setCurrentIndex(0);
	cvOriginImage.copyTo(cvImage);
	cv::imshow("image", cvOriginImage);
}

void ImageWorkShop::slotGauss(){
	if(!loadImage){
		return;
	}
	ui.comboBox_Lomo->setCurrentIndex(0);
	active = NONE;
	int radius = ui.horizontalSliderGauss->value();
	cvOriginImage.copyTo(cvImage);
	gaussFilter.gaussFilter(cvImage, radius);

	QString s = QString::number(radius);
	ui.lineEditGauss->setText(s);
	cv::imshow("image", cvImage);
}

void ImageWorkShop::slotCompare(){
	if(!loadImage){
		return;
	}
	cv::imshow("origin image", cvOriginImage);
}

void ImageWorkShop::slotSharp(){
	if(!loadImage){
		return;
	}
	ui.comboBox_Lomo->setCurrentIndex(0);
	active = NONE;
	cvOriginImage.copyTo(cvImage);
	imageSharp.sharp(cvImage);
	cv::imshow("image", cvImage);
}


void ImageWorkShop::slotSaturation(){
	if(!loadImage){
		return;
	}
	ui.comboBox_Lomo->setCurrentIndex(0);
	active = NONE;
	int inc = ui.horizontalSliderSaturation->value() - 50;
	cvOriginImage.copyTo(cvImage);
	saturation.saturation(cvImage, inc * 1.0 / 100);

	QString s = QString::number(inc * 1.0 / 100,'f',2);
	ui.lineEditSaturation->setText(s);
	cv::imshow("image", cvImage);
}

void ImageWorkShop::slotImageCopyRadius(){
	if(!loadImage){
		return;
	}
	active = PATCHMATCH;
	myPatchMatch.interection = ImageCopy;
	myPatchMatch.ImageCopyMode = ADDTARGET;
	myPatchMatch.ImageCopyRadius = ui.horizontalSliderBrushRadiusImageCopy->value();
	ui.lineEditBrushRadiusImageCopy->setText(QString::number(myPatchMatch.ImageCopyRadius));
}

void ImageWorkShop::slotImageCopyMovePatch(){
	if(!loadImage){
		return;
	}
	myPatchMatch.ImageCopyMode = MOVE;
	active = PATCHMATCH;
	myPatchMatch.interection = ImageCopy;
	myPatchMatch.ImageCopyMode = MOVE;
}


void ImageWorkShop::slotImageInpaintRadius(){
	if(!loadImage){
		return;
	}
	active = PATCHMATCH;
	myPatchMatch.interection = INPAINT;
	myPatchMatch.ImageInpaintMode = INPAINT_HOLE;
	myPatchMatch.ImageInpaintRadius = ui.horizontalSliderBrushRadiusImageInpaintHole->value();
	ui.lineEditBrushRadiusImageInpaintHole->setText(QString::number(myPatchMatch.ImageInpaintRadius));
}

void ImageWorkShop::slotImageInpaintRestrictRadius(){
	if(!loadImage){
		return;
	}
	active = PATCHMATCH;
	myPatchMatch.interection = INPAINT;
	myPatchMatch.ImageInpaintMode = INPAINT_RESTRICT;
	myPatchMatch.ImageInpaintRestrictRadius = ui.horizontalSliderBrushRadiusImageInpaintHoleRestrict->value();
	ui.lineEditBrushRadiusImageInpaintHoleRestrict->setText(QString::number(myPatchMatch.ImageInpaintRestrictRadius));
}

void ImageWorkShop::slotImageInpaintResult(){
	if(!loadImage){
		return;
	}
	active = PATCHMATCH;
	myPatchMatch.interection = INPAINT;
	myPatchMatch.ImageInpaintMode = INPAINT_NO;
	myPatchMatch.ImageInpaintModeWhenWork = INPAINT_HOLE;
	cvOriginImage.copyTo(cvImage);

	//red
	for(int row = 0; row < cvImage.rows; row++){
		for(int col = 0; col < cvImage.cols; col++){
			if(cvImage.at<cv::Vec3b>(row, col)[2] > 230 && cvImage.at<cv::Vec3b>(row, col)[0] + cvImage.at<cv::Vec3b>(row, col)[1] < 20)
				myPatchMatch.ImageInpaintMask[row * cvImage.cols + col] = 1;
		}
	}
	//black
	for(int row = 0; row < cvImage.rows; row++){
		for(int col = 0; col < cvImage.cols; col++){
			if(cvImage.at<cv::Vec3b>(row, col) == cv::Vec3b(0,0,0))
				myPatchMatch.ImageInpaintMask[row * cvImage.cols + col] = 1;
		}
	}

	myPatchMatch.erodeMask(cvImage.rows, cvImage.cols);

	for(int row = 0; row < cvImage.rows; row++){
		for(int col = 0; col < cvImage.cols; col++){
			if(myPatchMatch.ImageInpaintMask[row * cvImage.cols + col] > 0){
				cvImage.at<cv::Vec3b>(row, col).val[0] = 0;
				cvImage.at<cv::Vec3b>(row, col).val[1] = 0;
				cvImage.at<cv::Vec3b>(row, col).val[2] = 0;
			}
		}
	}

	//imshow("image",cvImage);
	//cvWaitKey();
	cv::Mat temp;
	cvImage.copyTo(temp);
	switch(myPatchMatch.mode){
	case CPU:
		myPatchMatch.imageInpaint(temp, cvImage);
		break;
	case GPU_JUMPFLOODING:
		myPatchMatch.imageInpaintCL_JF(temp, cvImage);
		break;
	}
	myPatchMatch.zeroInpaintMask(cvImage.rows * cvImage.cols, false);
	cv::imshow("image", cvImage);
	//waitKey();

	myPatchMatch.ImageInpaintModeWhenWork = INPAINT_NO;
}

void ImageWorkShop::slotImageInpaintRestrictResult(){
	if(!loadImage){
		return;
	}
	active = PATCHMATCH;
	myPatchMatch.interection = INPAINT;
	myPatchMatch.ImageInpaintMode = INPAINT_NO;
	myPatchMatch.ImageInpaintModeWhenWork = INPAINT_RESTRICT;
	cvOriginImage.copyTo(cvImage);

	char* holeInRestrict = new char[cvImage.rows * cvImage.cols];


	
	//get hole
	for(int row = 0; row < cvImage.rows; row++){
		for(int col = 0; col < cvImage.cols; col++){
			if(cvImage.at<cv::Vec3b>(row, col) == cv::Vec3b(0,0,0))
				myPatchMatch.ImageInpaintMask[row * cvImage.cols + col] = 1;
		}
	}
	for(int row = 0; row < cvImage.rows; row++){
		for(int col = 0; col < cvImage.cols; col++){
			if(cvImage.at<cv::Vec3b>(row, col)[2] > 230 && cvImage.at<cv::Vec3b>(row, col)[0] + cvImage.at<cv::Vec3b>(row, col)[1] < 20)
				myPatchMatch.ImageInpaintMask[row * cvImage.cols + col] = 1;
		}
	}
	myPatchMatch.erodeMask(cvImage.rows, cvImage.cols);


	//only mark pixel "hole" when this pixel is a hole and is in restrict,  then remove the restrict mark on the pixel
	// and removel the hole attrib of the pixel in ImageInpaintMask
	for(int row = 0; row < cvImage.rows; row++){
		for(int col = 0; col < cvImage.cols; col++){
			if(myPatchMatch.ImageInpaintMask[row * cvImage.cols + col] == 1 && myPatchMatch.InpaintRestrictMask[row * cvImage.cols + col] == 1){
				holeInRestrict[row * cvImage.cols + col] = 1;
				myPatchMatch.InpaintRestrictMask[row * cvImage.cols + col] = 0;
				myPatchMatch.ImageInpaintMask[row * cvImage.cols + col] = 0;
			}
			else{
				holeInRestrict[row * cvImage.cols + col] = 0;
			}
		}
	}

	for(int row = 0; row < cvImage.rows; row++){
		for(int col = 0; col < cvImage.cols; col++){
			if(myPatchMatch.ImageInpaintMask[row * cvImage.cols + col] > 0){
				cvImage.at<cv::Vec3b>(row, col).val[0] = 0;
				cvImage.at<cv::Vec3b>(row, col).val[1] = 0;
				cvImage.at<cv::Vec3b>(row, col).val[2] = 0;
			}
		}
	}

	cv::Mat temp;
	cvImage.copyTo(temp);

	
	//myPatchMatch.imageInpaintWithRestrict(temp, cvImage, holeInRestrict);
	switch(myPatchMatch.mode){
	case CPU:
		myPatchMatch.imageInpaintWithRestrict(temp, cvImage, holeInRestrict);
		break;
	case GPU_JUMPFLOODING:
		myPatchMatch.imageInpaintWithRestrictCL_JF(temp, cvImage, holeInRestrict);
		break;
	}

	//zero InpaintRestrictMask
	for(int row = 0; row < cvImage.rows; row++){
		for(int col = 0; col < cvImage.cols; col++){
			myPatchMatch.InpaintRestrictMask[row * cvImage.cols + col] = 0;
		}
	}
	//free memory of holeInRestrict
	delete holeInRestrict;

	myPatchMatch.ImageInpaintModeWhenWork = INPAINT_NO;
}

void ImageWorkShop::slotImageInpaintRestrictResultOut(){
	if(!loadImage){
		return;
	}
	active = PATCHMATCH;
	myPatchMatch.interection = INPAINT;
	myPatchMatch.ImageInpaintMode = INPAINT_NO;
	myPatchMatch.ImageInpaintModeWhenWork = INPAINT_RESTRICT;
	cvOriginImage.copyTo(cvImage);

	char* holeInRestrict = new char[cvImage.rows * cvImage.cols];


	
	//get hole
	for(int row = 0; row < cvImage.rows; row++){
		for(int col = 0; col < cvImage.cols; col++){
			if(cvImage.at<cv::Vec3b>(row, col) == cv::Vec3b(0,0,0))
				myPatchMatch.ImageInpaintMask[row * cvImage.cols + col] = 1;
		}
	}
	for(int row = 0; row < cvImage.rows; row++){
		for(int col = 0; col < cvImage.cols; col++){
			if(cvImage.at<cv::Vec3b>(row, col)[2] > 230 && cvImage.at<cv::Vec3b>(row, col)[0] + cvImage.at<cv::Vec3b>(row, col)[1] < 20)
				myPatchMatch.ImageInpaintMask[row * cvImage.cols + col] = 1;
		}
	}
	myPatchMatch.erodeMask(cvImage.rows, cvImage.cols);


	//only mark pixel "hole" when this pixel is a hole and is in restrict,  then remove the restrict mark on the pixel
	// and removel the hole attrib of the pixel in ImageInpaintMask
	for(int row = 0; row < cvImage.rows; row++){
		for(int col = 0; col < cvImage.cols; col++){
			if(myPatchMatch.ImageInpaintMask[row * cvImage.cols + col] == 1){ 
				holeInRestrict[row * cvImage.cols + col] = 1;
				myPatchMatch.InpaintRestrictMask[row * cvImage.cols + col] = 0;
				myPatchMatch.ImageInpaintMask[row * cvImage.cols + col] = 0;
			}
			
		}
	}


	cv::Mat temp;
	cvImage.copyTo(temp);
	switch(myPatchMatch.mode){
	case CPU:
		myPatchMatch.imageInpaintWithRestrict(temp, cvImage, holeInRestrict);
		break;
	case GPU_JUMPFLOODING:
		myPatchMatch.imageInpaintWithRestrictCL_JF(temp, cvImage, holeInRestrict);
		break;
	}
	myPatchMatch.zeroInpaintMask(cvImage.rows * cvImage.cols, false);


	//zero InpaintRestrictMask
	for(int row = 0; row < cvImage.rows; row++){
		for(int col = 0; col < cvImage.cols; col++){
			myPatchMatch.InpaintRestrictMask[row * cvImage.cols + col] = 0;
		}
	}
	//free memory of holeInRestrict
	delete holeInRestrict;

	myPatchMatch.ImageInpaintModeWhenWork = INPAINT_NO;
}
void ImageWorkShop::slotRrodeRadius(){
	if(!loadImage){
		return;
	}
	myPatchMatch.erodeRadius = ui.spinBoxErodeRadius->value();
}
void ImageWorkShop::slotWhiteBalance(){
	if(!loadImage){
		return;
	}
	ui.comboBox_Lomo->setCurrentIndex(0);
	active = NONE;
	int ratio = ui.horizontalSliderBalance->value();
	cvOriginImage.copyTo(cvImage);
	whiteBalance.perfectReflact(cvImage, ratio);
	QString s = QString::number(ratio);
	ui.lineEditBalance->setText(s);
	cv::imshow("image", cvImage);
}

void ImageWorkShop::slotLiquifySetRadius(){
	if(active != LIQUIFY)
		return;
	ui.comboBox_Lomo->setCurrentIndex(0);
	int radius = ui.spinBoxLiquifyRadius->value();
	liquify.radius = radius;
	liquify.genMask();
}

void ImageWorkShop::slotLiquify(){
	if(!loadImage){
		return;
	}
	ui.comboBox_Lomo->setCurrentIndex(0);
	active = LIQUIFY;
	cvOriginImage.copyTo(cvImage);
	liquify.image.rows = cvImage.rows;
	liquify.image.cols = cvImage.cols;
	liquify.image.dims = cvImage.dims;
	cv::imshow("image", cvImage);
}

void ImageWorkShop::slotLomo(){
	if(!loadImage){
		return;
	}
	active = NONE;
	int mode = ui.comboBox_Lomo->currentIndex();
	cvOriginImage.copyTo(cvImage);
	switch(mode){
	//case NOMODE:
	//	break;
	case SEPIA:
		//lomo.Sepia(cvImage);
		lomo.Sepia2(cvImage);
		break;
	case PENCIL:
		lomo.Pencil(cvImage);
		break;
	case CARVING:
		lomo.Carving(cvImage);
		break;
	case LOMO1:
		lomo.lomoStyle1(cvImage);
		vignette.vignette(cvImage, 1.1);
		break;
	default:
		break;
	}
	cv::imshow("image", cvImage);
}

void ImageWorkShop::slotLomoR(){
	if(!loadImage){
		return;
	}
	active = NONE;
	cvOriginImage.copyTo(cvImage);
	int midR = ui.horizontalSliderLomoR->value();
	int midG = ui.horizontalSliderLomoG->value();
	int midB = ui.horizontalSliderLomoB->value();
	unsigned char TB[256];
	unsigned char TG[256];
	unsigned char TR[256];
	lomo.GetLevelTable(TR, 0, midR, 255, 0, 255);
	lomo.GetLevelTable(TG, 0, midG, 255, 0, 255);
	lomo.GetLevelTable(TB, 0, midB, 255, 0, 255);
	lomo.Curve(cvImage, TB, TG, TR);
	QString s = QString::number(midR);
	ui.lineEditLomoR->setText(s);
	cv::imshow("image", cvImage);
}
void ImageWorkShop::slotLomoG(){
	if(!loadImage){
		return;
	}
	active = NONE;
	cvOriginImage.copyTo(cvImage);
	int midR = ui.horizontalSliderLomoR->value();
	int midG = ui.horizontalSliderLomoG->value();
	int midB = ui.horizontalSliderLomoB->value();
	unsigned char TB[256];
	unsigned char TG[256];
	unsigned char TR[256];
	lomo.GetLevelTable(TR, 0, midR, 255, 0, 255);
	lomo.GetLevelTable(TG, 0, midG, 255, 0, 255);
	lomo.GetLevelTable(TB, 0, midB, 255, 0, 255);
	lomo.Curve(cvImage, TB, TG, TR);
	QString s = QString::number(midG);
	ui.lineEditLomoG->setText(s);
	cv::imshow("image", cvImage);
}
void ImageWorkShop::slotLomoB(){
	if(!loadImage){
		return;
	}
	active = NONE;
	cvOriginImage.copyTo(cvImage);
	int midR = ui.horizontalSliderLomoR->value();
	int midG = ui.horizontalSliderLomoG->value();
	int midB = ui.horizontalSliderLomoB->value();
	unsigned char TB[256];
	unsigned char TG[256];
	unsigned char TR[256];
	lomo.GetLevelTable(TR, 0, midR, 255, 0, 255);
	lomo.GetLevelTable(TG, 0, midG, 255, 0, 255);
	lomo.GetLevelTable(TB, 0, midB, 255, 0, 255);
	lomo.Curve(cvImage, TB, TG, TR);
	QString s = QString::number(midB);
	ui.lineEditLomoB->setText(s);
	cv::imshow("image", cvImage);
}

void ImageWorkShop::slotExfoliating(){
	if(!loadImage){
		return;
	}
	active = NONE;
	cvOriginImage.copyTo(cvImage);

	cv::Mat image(cvImage.size(), CV_64F);
	cvImage.copyTo(image);

	int vdev = ui.spinBox_Vdev->value();
	int sdev = ui.spinBox_Sdev->value();
	faceBeautification.bilateralFilter(image, vdev, 1.0 / sdev);
	//faceBeautification.genFaceMask(cvImage);

	cv::Mat GaussMask(skinMask.size(), CV_64F);
	skinMask.copyTo(GaussMask);
	int thre = ui.horizontalSliderThreshold->value();
	gaussFilter.gaussFilter(GaussMask, thre);

	//cv::imshow("gaussMask", GaussMask);

	int k;
	for(int row = 0; row < cvImage.rows; row++){
		for(int col = 0; col < cvImage.cols; col++){
			//if(skinMask.at<cv::Vec3b>(row, col).val[0] == 255)
			//	cvImage.at<cv::Vec3b>(row, col) = image.at<cv::Vec3b>(row, col);
			int gauss = GaussMask.at<cv::Vec3b>(row, col).val[0];
			if(gauss > 0){
				k = (cvImage.at<cv::Vec3b>(row, col).val[0] * 158 + 
						 cvImage.at<cv::Vec3b>(row, col).val[1] * 75 + 
						 cvImage.at<cv::Vec3b>(row, col).val[2] * 55) / (158 + 72 + 55);
				if(k > whitenThreshold){
					int r, g, b;
					b = image.at<cv::Vec3b>(row, col).val[0] * gauss + cvImage.at<cv::Vec3b>(row, col).val[0] * (255 - gauss);
					g = image.at<cv::Vec3b>(row, col).val[1] * gauss + cvImage.at<cv::Vec3b>(row, col).val[1] * (255 - gauss);
					r = image.at<cv::Vec3b>(row, col).val[2] * gauss + cvImage.at<cv::Vec3b>(row, col).val[2] * (255 - gauss);

					cvImage.at<cv::Vec3b>(row, col).val[0] = b / 255;
					cvImage.at<cv::Vec3b>(row, col).val[1] = g / 255;
					cvImage.at<cv::Vec3b>(row, col).val[2] = r / 255;
				}
					//cvImage.at<cv::Vec3b>(row, col) = (image.at<cv::Vec3b>(row, col) * g + cvImage.at<cv::Vec3b>(row, col) * (255 - g);
			}
		}
	}
	cv::imshow("image", cvImage);
}

void ImageWorkShop::slotWhitenSkin(){
	if(!loadImage){
		return;
	}
	active = NONE;
	cvOriginImage.copyTo(cvImage);
	int beta = ui.horizontalSliderWhitenSkin->value();

	cv::Mat image(cvImage.size(), CV_64F);
	cvImage.copyTo(image);

	faceBeautification.curve(image, beta);
	QString s = QString::number(beta);
	ui.lineEditWhitenSkin->setText(s);


	cv::Mat GaussMask(skinMask.size(), CV_64F);
	skinMask.copyTo(GaussMask);
	int thre = ui.horizontalSliderThreshold->value();
	gaussFilter.gaussFilter(GaussMask, thre);

	//cv::imshow("gaussMask", GaussMask);

	int k;
	for(int row = 0; row < cvImage.rows; row++){
		for(int col = 0; col < cvImage.cols; col++){
			int gauss = GaussMask.at<cv::Vec3b>(row, col).val[0];
			if(gauss > 0){
				k = (cvImage.at<cv::Vec3b>(row, col).val[0] * 158 + 
						 cvImage.at<cv::Vec3b>(row, col).val[1] * 75 + 
						 cvImage.at<cv::Vec3b>(row, col).val[2] * 55) / (158 + 72 + 55);
				if(k > whitenThreshold){
					int r, g, b;
					b = image.at<cv::Vec3b>(row, col).val[0] * gauss + cvImage.at<cv::Vec3b>(row, col).val[0] * (255 - gauss);
					g = image.at<cv::Vec3b>(row, col).val[1] * gauss + cvImage.at<cv::Vec3b>(row, col).val[1] * (255 - gauss);
					r = image.at<cv::Vec3b>(row, col).val[2] * gauss + cvImage.at<cv::Vec3b>(row, col).val[2] * (255 - gauss);

					cvImage.at<cv::Vec3b>(row, col).val[0] = b / 255;
					cvImage.at<cv::Vec3b>(row, col).val[1] = g / 255;
					cvImage.at<cv::Vec3b>(row, col).val[2] = r / 255;
				}
					//cvImage.at<cv::Vec3b>(row, col) = (image.at<cv::Vec3b>(row, col) * g + cvImage.at<cv::Vec3b>(row, col) * (255 - g);
			}
		}
	}
	cv::imshow("image", cvImage);
}

//void ImageWorkShop::slotDrawSkinMask(){
//	if(!loadImage){
//		return;
//	}
//	active = SKINMASK;
//}
void ImageWorkShop::slotDrawSkinMaskRadius(){
	if(!loadImage){
		return;
	}
	active = SKINMASK;
	//cvOriginImage.copyTo(cvImage);
	int radius = ui.horizontalSliderWhitenSkinMask->value();
	skinMaskRadius = radius;
	//faceBeautification.curve(cvImage, beta);
	QString s = QString::number(radius);
	ui.lineEditWhitenSkinMask->setText(s);
	//cv::imshow("image", cvImage);
}

void ImageWorkShop::slotWhitenThreshold(){
	if(!loadImage){
		return;
	}
	active = SKINMASK;
	int thre = ui.horizontalSliderThreshold->value();
	whitenThreshold = thre;

	QString s = QString::number(thre);
	ui.lineEditWhitenThreshold->setText(s);
	cv::imshow("image", cvImage);
}

void ImageWorkShop::slotColorMapSource(){
	QString filename;
    filename= QFileDialog::getOpenFileName(this,
                                          tr("open color map source"), "",
                                          tr("Images (*.png *.bmp *.jpeg *.jpg)")); 
	if(filename.isEmpty())
    { 
		return;
    } 
	else {
		std::string str = filename.toLocal8Bit();
		colorMapSource = cv::imread(str) ;
			
		cv::imshow("color map source", colorMapSource) ;
    }
}

void ImageWorkShop::slotColorMapTarget(){
	QString filename;
    filename= QFileDialog::getOpenFileName(this,
                                          tr("open color map target"), "",
                                          tr("Images (*.png *.bmp *.jpeg *.jpg)")); 
	if(filename.isEmpty())
    { 
		return;
    } 
	else {
		std::string str = filename.toLocal8Bit();
		colorMapTarget = cv::imread(str) ;
			
		cv::imshow("color map target", colorMapTarget) ;
    }
}

void ImageWorkShop::slotColorMapResult(){
	if(colorMapSource.rows == 0 || colorMapTarget.rows == 0)
		return;
	ColorTransfer clt(colorMapSource, colorMapTarget);  
	cv::imshow("color map result", clt.result) ;

}
void ImageWorkShop::slotColorMapGrayToRGB(){
	if(colorMapSource.rows == 0 || colorMapTarget.rows == 0)
		return;
	ColorTransfer clt;  
	clt.localVarianceTransfer(colorMapSource, colorMapTarget);
}

void ImageWorkShop::addToSkinMask(int ax, int ay){
	int radius = skinMaskRadius;
	int centerX = ax;
	int centerY = ay;
	for(int row = -radius; row < radius + 1; row++){
		for(int col = -radius; col < radius + 1; col++){
			if(centerX + col < 0 || centerY + row < 0 || centerX + col >= skinMask.cols || centerY + row >= skinMask.rows)
				continue;
			if(row * row + col * col < radius * radius){	
				skinMask.at<cv::Vec3b>(centerY + row, centerX + col).val[0] = 255;
				skinMask.at<cv::Vec3b>(centerY + row, centerX + col).val[1] = 255;
				skinMask.at<cv::Vec3b>(centerY + row, centerX + col).val[2] = 255;
			}
		}
	}
	//cv::imshow("SkinMask", skinMask);
}

void ImageWorkShop::addMaskToImage(cv::Mat &img){
	for(int row = 0; row < img.rows; row++){
		for(int col = 0; col < img.cols; col++){
			if(skinMask.at<cv::Vec3b>(row, col).val[0] == 255){	
				img.at<cv::Vec3b>(row, col).val[2] = (img.at<cv::Vec3b>(row, col).val[2] + 255) / 2;
			}
		}
	}
}

void ImageWorkShop::addCircle(cv::Mat &image, int ax, int ay){
	int radius = skinMaskRadius;
	int centerX = ax;
	int centerY = ay;
	for(int row = -radius; row < radius + 1; row++){
		for(int col = -radius; col < radius + 1; col++){
			if(centerX + col < 0 || centerY + row < 0 || centerX + col >= image.cols || centerY + row >= image.rows)
				continue;
			if(row * row + col * col < radius * radius){	
				image.at<cv::Vec3b>(centerY + row, centerX + col).val[0] = 0;
				image.at<cv::Vec3b>(centerY + row, centerX + col).val[1] = 0;
				image.at<cv::Vec3b>(centerY + row, centerX + col).val[2] = 150;
			}
		}
	}
}
void ImageWorkShop::zeroImage(cv::Mat &img){
	for(int row = 0; row < img.rows; row++){
		for(int col = 0; col < img.cols; col++){
			img.at<cv::Vec3b>(row, col).val[0] = 0;
			img.at<cv::Vec3b>(row, col).val[1] = 0;
			img.at<cv::Vec3b>(row, col).val[2] = 0;
		}
	}
}


void ImageWorkShop::slotPatchMatchIfGpu(){
	active = NONE;
	myPatchMatch.mode = (PatchMatchMode)ui.comboBox_ifGpu->currentIndex();
}

void ImageWorkShop::slotClearRestrict(){
	if(!loadImage){
		return;
	}
	active = NONE;
	int length = cvImage.rows * cvImage.cols;
	for(int i = 0; i < length; i++){
		myPatchMatch.InpaintRestrictMask[i] = 0;
	}
	Mat temp = cvImage.clone();
	myPatchMatch.showImageInpaintMaskImage(temp);
	imshow("image", temp);
}
void ImageWorkShop::slotClearRegion(){
	if(!loadImage){
		return;
	}
	active = NONE;
	myPatchMatch.zeroImageCopyMask(cvImage.rows * cvImage.cols, false);

	Mat temp = cvImage.clone();
	myPatchMatch.showImageCopyMaskImage(temp);
	imshow("image", temp);
}
ImageWorkShop *imageworkshop;
bool mouseDown = false;;
void onMouse(int event,int ax,int ay,int flags,void* param )
{
	bool maskImageCopyToCvImage = false;
	cv::Mat img_circle;
	cv::Mat maskImage;
	//cv::imshow("asd", imageworkshop->cvImage);
	//waitKey(33);
	imageworkshop->cvImage.copyTo(maskImage);
    switch (event){  
	case CV_EVENT_MOUSEMOVE:
		if(mouseDown){
			switch(imageworkshop->active){
			case NONE:
				break;
			case LIQUIFY:
				imageworkshop->cvImage.copyTo(imageworkshop->liquify.image);
				imageworkshop->liquify.liquify(imageworkshop->liquify.image, 0, ax, ay);
				imageworkshop->liquify.addCircle(img_circle);
				cv::imshow("image", img_circle);
				break;
			case SKINMASK:
				imageworkshop->addToSkinMask(ax, ay);
				break;
			case PATCHMATCH:
				switch(imageworkshop->myPatchMatch.interection){
				case ImageCopy:
					imageworkshop->myPatchMatch.dealWithImageCopyMouseMoveEvent(ax, ay, maskImage);
					break;
				case INPAINT:
					imageworkshop->myPatchMatch.dealWithImageInpaintMouseMoveEvent(ax, ay, maskImage);
					break;
				}
				break;
			}
		}
        break;  
	case CV_EVENT_LBUTTONDOWN:	
		switch(imageworkshop->active){
		case NONE:
			break;
		case LIQUIFY:
			imageworkshop->liquify.centerX = ax;
			imageworkshop->liquify.centerY = ay;
			imageworkshop->cvImage.copyTo(imageworkshop->liquify.image);
			break;
		case SKINMASK:
			imageworkshop->addToSkinMask(ax, ay);
			break;
		case PATCHMATCH:
			switch(imageworkshop->myPatchMatch.interection){
			case ImageCopy:
				imageworkshop->myPatchMatch.dealWithImageCopyMouseMoveButtonDown(ax, ay, maskImage);
				break;
			case INPAINT:
				imageworkshop->myPatchMatch.dealWithImageInpaintMouseButtonDown(ax, ay, maskImage);
				break;
			}
			break;
		}
		mouseDown = true;
        break;  
    case CV_EVENT_LBUTTONUP:
		if(mouseDown){
			switch(imageworkshop->active){
			case NONE:
				break;
			case LIQUIFY:
				imageworkshop->liquify.image.copyTo(imageworkshop->cvImage);
				cv::imshow("image", imageworkshop->cvImage);
				break;
			case PATCHMATCH:
				switch(imageworkshop->myPatchMatch.interection){
				case ImageCopy:
					if(imageworkshop->myPatchMatch.dealWithImageCopyMouseMoveButtonUp(ax, ay, maskImage)){
						imageworkshop->myPatchMatch.interection = NO;
						maskImageCopyToCvImage = true;
						maskImage.copyTo(imageworkshop->cvImage);
					}
					break;
				case INPAINT:
					imageworkshop->myPatchMatch.dealWithImageInpaintMouseButtonUp(ax, ay, maskImage);
					break;
				}
				break;
			}

		}
		mouseDown = false;
        break;  
    }

	if(imageworkshop->active == SKINMASK){
		imageworkshop->addMaskToImage(maskImage);
		imageworkshop->addCircle(maskImage, ax, ay);
		cv::imshow("image", maskImage);
	}

	if(imageworkshop->active == PATCHMATCH){
		if(imageworkshop->myPatchMatch.interection == ImageCopy){
			imageworkshop->myPatchMatch.showImageCopyMaskImage(maskImage);
			if(imageworkshop->myPatchMatch.ImageCopyMode == ADDTARGET){
				imageworkshop->myPatchMatch.addCircleToImage(maskImage, ax, ay);
				cv::imshow("image", maskImage);
			}
			if(imageworkshop->myPatchMatch.ImageCopyMode == MOVE){
				if(!imageworkshop->myPatchMatch.ImageCopyMoveStart)
					cv::imshow("image", maskImage);
			}
		}

		if(imageworkshop->myPatchMatch.interection == INPAINT){
			if(imageworkshop->myPatchMatch.ImageInpaintMode != INPAINT_NO){
				imageworkshop->myPatchMatch.showImageInpaintMaskImage(maskImage);
				imageworkshop->myPatchMatch.showImageInpaintRestrictMaskImage(maskImage);
				imageworkshop->myPatchMatch.addCircleToImageInpaint(maskImage, ax, ay);
				cv::imshow("image", maskImage);
			}
		}
	}
}

