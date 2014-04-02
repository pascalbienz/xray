#include "xray_gui.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	XRAY_gui w;
	w.show();
	return a.exec();
}
