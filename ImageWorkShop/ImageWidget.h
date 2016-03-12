#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H
#include<qwidget.h>
#include<qpainter.h>
#include <QMouseEvent>
#include <qimage.h>
//#include <qimagereader.h>

class ImageWidget : public QWidget
{
	Q_OBJECT
public:
	ImageWidget(QWidget *parent = 0);
	~ImageWidget();

protected:
	//void paintEvent(QPaintEvent *event);
	//void mousePressEvent(QMouseEvent *event);

private:
	void draw();
	void readRawData();
	
	QImage image;
	QImage origin_image;


public slots:
	void loadfile();
	void savefile();

};

#endif