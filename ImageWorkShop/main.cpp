#include "imageworkshop.h"
#include <QtWidgets/QApplication>
extern ImageWorkShop *imageworkshop;
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ImageWorkShop w;
	imageworkshop = &w;
	w.show();
	return a.exec();
}
